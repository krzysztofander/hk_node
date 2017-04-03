/************************************************************************************************************************
Copyright (c) 2016, Krzysztof Bartczak
All rights reserved.
Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this list of conditions and the
following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
following disclaimer in the documentation and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote
products derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
************************************************************************************************************************/

#include <Arduino.h>
#include "hk_node.h"
#include "serial.h"

//  preable is any series of characters
//+ '!' + '#'

HKSerial::PreableState  HKSerial::g_preableState = HKSerial::PreableState::none;
int8_t HKSerial::g_preableFinishTime = 8;
int8_t HKSerial::g_preambleInactivityTime = 0;
char HKSerial::g_BTName[12] = { 'H','K','N','o','d','e','D','e','f','l','t', '\x0' };
void HKSerial::traverseSM(char charRead)
{
    if (HKSerial::PreableState::finished != g_preableState)
    {
        switch (charRead)
        {
            case '!':
                g_preableState = HKSerial::PreableState::exclamation;
                break;
            case '#':
                if (g_preableState == HKSerial::PreableState::exclamation)
                {
                    g_preableState = HKSerial::PreableState::finished;
                    //here the SM gets locked e.g. it will stay in this state
                    //until reset
                }
                else
                {
                    // `#` got recieved, but prior to it there was no `!`
                    // so it is a random series of chars.
                    g_preableState = HKSerial::PreableState::nonRecognizedChar;
                }
                break;
            case 'O':
                g_preableState = HKSerial::PreableState::okLost_O;
                break;
            case 'S':
                if (g_preableState == HKSerial::PreableState::okLost_O)
                {
                    g_preableState = HKSerial::PreableState::okLost_S;
                }
                else
                {
                    g_preableState = HKSerial::PreableState::nonRecognizedChar;
                }
                break;
            case 'T':
                if (g_preableState == HKSerial::PreableState::okLost_S)
                {
                    g_preableState = HKSerial::PreableState::okLost_T;
                }
                else
                {
                    g_preableState = HKSerial::PreableState::nonRecognizedChar;
                }
                break;
                
           default:
                g_preableState = HKSerial::PreableState::nonRecognizedChar;
                break;
        }
    }
  
}

void HKSerial::resetSM()
{
    g_preableState = HKSerial::PreableState::none;
    g_preambleInactivityTime = 0;
}

bool HKSerial::preambleRecieved()
{
    return  g_preableState == HKSerial::PreableState::finished;
}

void HKSerial::sendBTCommand(const char * command, int8_t size, bool waitResponse)
{
    char buffer[24];
    Serial.begin(9600);
    Serial.write((const uint8_t*)command, size);
    if (waitResponse)
    {
        Serial.setTimeout(800);
        Serial.readBytes(buffer, NUM_ELS(buffer));
    }
}

void HKSerial::nextLoop(uint8_t secondsCnt)
{
    static uint8_t prevSecondsCnt = 255; 
    if (secondsCnt != prevSecondsCnt)
    {
        prevSecondsCnt =  secondsCnt;
        activate();
        if (isActive()  && !preambleRecieved())
        {
            //state when we have something, but not a preable
            g_preambleInactivityTime++;
        }
        else
        {
            g_preambleInactivityTime = 0;
        }
        
        if (g_preambleInactivityTime >= g_preableFinishTime)
        {
            //N seconds has passed since activity was detected
            //we need to put serial to sleep
            resetSM();
            g_preambleInactivityTime = 0;
        }
    }
}
#include "string.h"
void HKSerial::BTinit()
{
    //init bluetooth
    sendBTCommand("AT+RENEW", 8,true);
    sendBTCommand("AT+MODE1", 8,true);
    sendBTCommand("AT+PIO11", 8,true);
    sendBTCommand("AT+ADVI4", 8,true);
    //at name here          ,true
    sendBTCommand("AT+NAME",  7,false);
    sendBTCommand(g_BTName ,  strlen(g_BTName),true);


    sendBTCommand("AT+RESET", 8,true);
    sendBTCommand("AT+MODE1", 8,true);
    sendBTCommand("AT+POWE3", 8,true);
    sendBTCommand("AT+NOTI1", 8,true);
    sendBTCommand("AT+SLEEP", 8,true);
}

void HKSerial::init()
{
    resetSM();
    BTinit();
    
}

void HKSerial::activate()
{
    if (!preambleRecieved())
    {
        if (Serial.available() > 0)
        {
            char nextChar = Serial.read();  //consume it, we are handling preable
            traverseSM(nextChar);
            g_preambleInactivityTime = 0;   //have processed something so reset timeout
        }
    }
    if (g_preableState == HKSerial::PreableState::okLost_T)
    {
        //put bluetooth to sleep here...
        sendBTCommand("AT+SLEEP", 8, true);
        resetSM();
    }
}

//Advances SM
//@returns true if anything got recieved
bool HKSerial::isActive()
{
    //Something was recived when:
    return g_preableState != HKSerial::PreableState::none;

}
uint8_t HKSerial::available()
{
    activate(); //will advance SM 
    if (preambleRecieved())
    {
        return Serial.available();
    }
    else
    {
         //return 0 until preamble is send
       return 0;
    }
}

void HKSerial::commandProcessed()
{
    resetSM();
}
uint8_t HKSerial::read()
{
    return Serial.read();
}

uint8_t HKSerial::peek()
{
    return Serial.peek();
}
uint8_t HKSerial::write(const uint8_t * buff, size_t size)
{
    return Serial.write(buff, size);
}
void HKSerial::flush()
{
    Serial.flush();
}
void HKSerial::end()
{
    Serial.end();
}
void HKSerial::begin(int speed)
{
    Serial.begin(speed);
}
