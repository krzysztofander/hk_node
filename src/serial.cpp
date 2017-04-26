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
#include "string.h"
#include "nv.h"
//  preable is any series of characters
//+ '!' + '#'

HKSerial::PreableState  HKSerial::g_preableState = HKSerial::PreableState::none;
int8_t HKSerial::g_preableFinishTime = 8;
int8_t HKSerial::g_preambleInactivityTime = 0;
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
    //@todo, expect only given characters.
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

void HKSerial::BTinit()
{
    char nameBuffer[static_cast<int>(NV::NVDataSize::nvBTName)];
    NV::read(NV::NVData::nvBTName, nameBuffer);
    uint8_t nameSize = strlen(nameBuffer);

    /*
    http://fab.cba.mit.edu/classes/863.15/doc/tutorials/programming/bluetooth/bluetooth40_en.pdf
    */
    //Krzysztof:
    /*  
    //Query/Set Advertising interval 
    sendBTCommand("AT+ADVIF", 8,true); 
        F: 0: 100ms
        1: 152.5 ms
        2: 211.25 ms
        3: 318.75 ms
        4: 417.5 ms
        5: 546.25 ms
        6: 760 ms
        7: 852.5 ms
        8: 1022.5 ms
        9: 1285 ms
        A: 2000ms
        B: 3000ms
        C: 4000ms
        D: 5000ms
        E: 6000ms
        F: 7000ms     
    //Query/Set Module work type 
    AT+IMME? 
        1: When module is powered on, only respond the AT Command, don’t do anything.
        until AT + START is received, or can use  AT+CON,AT+CONNL
        0: When power on, work immediately 
        **This command is only used for Central role. **
    AT+MODE?
        Mode 0: Before establishing a connection, you can use the AT command
            configuration module through UART.
            After established a connection, you can send data to remote side from
            each other.
        Mode 1: Before establishing a connection, you can use the AT command
            configuration module through UART.
            After established a connection, you can send data to remote side. Remote
            side can do fellows: 
                Send AT command configuration module.
                Collect PIO04 to the PIO11 pins input state of HM-10.
                Collect PIO03 pins input state of HM-11.
                Remote control PIO2, PIO3 pins output state of HM-10.
                Remote control PIO2 pin output state of HM-11.
                Send data to module UART port (not include any AT command and per
                package must less than 20 bytes).
        Mode 2: Before establishing a connection, you can use the AT command
            configuration module through UART.
            After established a connection, you can send data to remote side. Remote
            side can do fellows:
                Send AT command configuration module.
                Remote control PIO2 to PIO11 pins output state of HM-10.
                Remote control PIO2, PIO3 pins output state of HM-11.
                Send data to module UART port (not include any AT command and per
                package must less than 20 bytes). 

    //Query/Set Notify information 
    AT+NOTI? 
        If this value is set to 1, when link ESTABLISHED or LOSTED module will
        send OK+CONN or OK+LOST string through UART. 

    //Query/Set Module name 
    AT+NAME？
        Para1: module name, Max length is 12. 

    //Query/Set Module Power 
    AT+POWE? 
        3: 6dbm 

    //Restore all setup value to factory setup 
    AT+RENEW 

    //.Restart module
    AT+RESET 

    //Query/Set Master and Slaver Role
    AT+ROLE[para1] 
        0: Peripheral
        1: Central
        Default: 0 

    //Query Module into sleep mode 
    AT+SLEEP 
        Only support Peripheral role. 
    
    //Query/Set Module sleep type 
    AT+PWRM[para1] 
        0:Auto sleep
        1:don’t auto sleep 
        Default: 1 
        Only support peripheral role. 

    */

    //init bluetooth
    sendBTCommand("AT+RENEW", 8,true);
    sendBTCommand("AT+MODE0", 8,true);
    sendBTCommand("AT+ADVI8", 8,true);
    sendBTCommand("AT+NAME",  7,false);
        sendBTCommand(nameBuffer ,nameSize,true);
    sendBTCommand("AT+POWE3", 8,true);
    sendBTCommand("AT+NOTI1", 8,true);

    sendBTCommand("AT+RESET", 8,true);
    //Lets try to keep it default and go to sleep. Perhapts the LED will go off anyway
    //sendBTCommand("AT+PIO11", 8,true);  //put LED of when not commencted
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
