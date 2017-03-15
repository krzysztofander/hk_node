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
#include "supp.h"

//------------------------------------------------------------------
uint8_t Supp::prevTimeMod256 = 111; //some value not probable after reset

//@ Initalizes Support class. Reads the platform type from pins, if any
void Supp::init()
{
    //@todo we should read here the platform type and set the pins apriopriately
    
    if (getPlatformType() == TempMeasurer)
    {
        Serial.begin(9600);
        pinMode(LED_BUILTIN, OUTPUT);
        pinMode(AlPin1, OUTPUT);
        pinMode(AlPin2, OUTPUT);
        pinMode(AlPin3, OUTPUT);
        pinMode(AlPin4, OUTPUT);
        pinMode(AlPinBlue, OUTPUT);
        pinMode(buttonPin, INPUT);

        digitalWrite(AlPin1,    HIGH);  
        digitalWrite(AlPin2,    HIGH);  
        digitalWrite(AlPin3,    HIGH);  
        digitalWrite(AlPin4,    HIGH);  
        digitalWrite(AlPinBlue,    HIGH);  

        //Invocation:
        for (int i = 0; i < 10; i++)
        {
            digitalWrite(LED_BUILTIN, HIGH); 
            delay(40);                     
            digitalWrite(LED_BUILTIN, LOW);  
            delay(20);                       
        }
        digitalWrite(AlPinBlue, LOW);                                     delay(300);   
        digitalWrite(AlPinBlue, HIGH);    digitalWrite(AlPin1, LOW);      delay(300);    
        digitalWrite(AlPin1,    HIGH);    digitalWrite(AlPin2, LOW);      delay(300);    
        digitalWrite(AlPin2,    HIGH);    digitalWrite(AlPin3, LOW);      delay(300);    
        digitalWrite(AlPin3,    HIGH);    digitalWrite(AlPin4, LOW);      delay(300);    
        digitalWrite(AlPin4,    HIGH);      
       

#if 0
        for (int i = 0; i < 10; i++)
        {
            toggleBlue();
            digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
            delay(20);                       // wait for a second
            digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
            delay(20);                       // wait for a second
            alert(i, false);
        }
        //end of test
        Serial.println("hello world");
#endif
    }
    else
    {
        dbgAlert(1, true);
    }
}

 
//@returns Platform type
Supp::PlatformType Supp::getPlatformType()
{
    return Supp::TempMeasurer;
}

//@debug function, can do whatever
void Supp::dbgAlert(uint8_t alertVal, bool hold)
{
#if 0
    switch (alertVal)
    {
        case 14:  digitalWrite(AlPin1, LOW);  digitalWrite(AlPin2, LOW);  digitalWrite(AlPin3, LOW);  digitalWrite(AlPin4, HIGH); break; 
        case 13:  digitalWrite(AlPin1, LOW);  digitalWrite(AlPin2, LOW);  digitalWrite(AlPin3, HIGH);  digitalWrite(AlPin4, LOW); break; 
        case 12:  digitalWrite(AlPin1, LOW);  digitalWrite(AlPin2, LOW);  digitalWrite(AlPin3, HIGH);  digitalWrite(AlPin4, HIGH); break; 
        case 11:  digitalWrite(AlPin1, LOW);  digitalWrite(AlPin2, HIGH);  digitalWrite(AlPin3, LOW);  digitalWrite(AlPin4, LOW); break; 
        case 10:  digitalWrite(AlPin1, LOW);  digitalWrite(AlPin2, HIGH);  digitalWrite(AlPin3, LOW);  digitalWrite(AlPin4, HIGH); break; 
        case 9 :  digitalWrite(AlPin1, LOW);  digitalWrite(AlPin2, HIGH);  digitalWrite(AlPin3, HIGH);  digitalWrite(AlPin4, LOW); break; 
        case 8 :  digitalWrite(AlPin1, LOW);  digitalWrite(AlPin2, HIGH);  digitalWrite(AlPin3, HIGH);  digitalWrite(AlPin4, HIGH); break; 
        case 7 :  digitalWrite(AlPin1, HIGH);  digitalWrite(AlPin2, LOW);  digitalWrite(AlPin3, LOW);  digitalWrite(AlPin4, LOW); break; 
        case 6 :  digitalWrite(AlPin1, HIGH);  digitalWrite(AlPin2, LOW);  digitalWrite(AlPin3, LOW);  digitalWrite(AlPin4, HIGH); break; 
        case 5 :  digitalWrite(AlPin1, HIGH);  digitalWrite(AlPin2, LOW);  digitalWrite(AlPin3, HIGH);  digitalWrite(AlPin4, LOW); break; 
        case 4 :  digitalWrite(AlPin1, HIGH);  digitalWrite(AlPin2, LOW);  digitalWrite(AlPin3, HIGH);  digitalWrite(AlPin4, HIGH); break; 
        case 3 :  digitalWrite(AlPin1, HIGH);  digitalWrite(AlPin2, HIGH);  digitalWrite(AlPin3, LOW);  digitalWrite(AlPin4, LOW); break; 
        case 2 :  digitalWrite(AlPin1, HIGH);  digitalWrite(AlPin2, HIGH);  digitalWrite(AlPin3, LOW);  digitalWrite(AlPin4, HIGH); break; 
        case 1 :  digitalWrite(AlPin1, HIGH);  digitalWrite(AlPin2, HIGH);  digitalWrite(AlPin3, HIGH);  digitalWrite(AlPin4, LOW); break; 

        default:
        case 15:  digitalWrite(AlPin1, LOW);  digitalWrite(AlPin2, LOW);  digitalWrite(AlPin3, LOW);  digitalWrite(AlPin4, LOW); break; 

    }
    if (
        1 && 
        hold)
    {
        volatile uint8_t buttonState = 1;
        while (  buttonState ) 
        {
            digitalWrite(LED_BUILTIN, LOW);
            delay(10);             
            digitalWrite(LED_BUILTIN, HIGH);
            delay(50);             
            buttonState = digitalRead(buttonPin);
        }
        delay(10);
        while (  !buttonState ) 
        {
            digitalWrite(LED_BUILTIN, LOW);
            delay(50);             
            digitalWrite(LED_BUILTIN, HIGH);
            delay(10);             
            buttonState = digitalRead(buttonPin);
        }
        delay(10);
        digitalWrite(AlPin1, HIGH);  digitalWrite(AlPin2, HIGH);  digitalWrite(AlPin3, HIGH);  digitalWrite(AlPin4, HIGH); 

    }
    else
    {
        delay(10);
    }
#endif
}


//@ Reacts on main loop loops and on passing time
void Supp::mainLoopStart(uint8_t timeMod256)
{
    if (timeMod256 != prevTimeMod256)
    {
        prevTimeMod256 = timeMod256;
        switch (timeMod256 & 3)
        {
            case 0: digitalWrite(AlPin2, LOW);  digitalWrite(AlPin3, LOW); break;
            case 1: digitalWrite(AlPin2, LOW);  digitalWrite(AlPin3, HIGH);  break;
            case 2: digitalWrite(AlPin2, HIGH);  digitalWrite(AlPin3, HIGH); break;
            default: digitalWrite(AlPin2, HIGH);  digitalWrite(AlPin3, LOW); break;
        }
    }
    
}

//@ Power down indication
void Supp::aboutToPowerDown()
{
    digitalWrite(PowerUpDown, HIGH);
}

//@ Power up indication
void Supp::justPoweredUp()
{
    digitalWrite(PowerUpDown, LOW);  
}

//@ No Powerdown happened
void Supp::noPowerDownHappened()
{
    justPoweredUp();
}

//@ Reacts of action from the executor
void Supp::executorPreAction(ExecutorBase::EExecutors eexecutor)
{
    if (eexecutor == ExecutorBase::temperatureMeasurer)
    {
        digitalWrite(AlPinBlue, LOW);
        delay(30);
        digitalWrite(AlPinBlue, HIGH); //OFF
        delay(50);
        digitalWrite(AlPinBlue, LOW); //ON
    }
}

void Supp::executorPostAction(ExecutorBase::EExecutors eexecutor)
{
    if (eexecutor == ExecutorBase::temperatureMeasurer)
    {
        digitalWrite(AlPinBlue, HIGH);  //OFF
        delay(50);
        digitalWrite(AlPinBlue, LOW); //ON
        delay(30);
        digitalWrite(AlPinBlue, HIGH); //OFF
    }
}


//@ build in LED blinker
//@param pattern, a blink pattern 1-short blink, 0 period as long as there is  
// at least one more MSB bit set as 1
void Supp::blinkLed(uint8_t pattern)
{
    while(pattern)
    {
        if (pattern & 1 == 1)
        {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(40);
            digitalWrite(LED_BUILTIN, LOW);
            delay(60);
        }
        else
        {
            delay(100);
        }
        pattern >>= 1;
    }
    digitalWrite(LED_BUILTIN, 0);
}

//@a button state
bool Supp::isButtonPressed()
{
    volatile uint8_t buttonState = digitalRead(buttonPin);
    return !buttonState;

}


static bool greenState = 0;

void Supp::greenOn()
{
    greenState = 0;
    digitalWrite(AlPinGreen, greenState); //ON
}
void Supp::greenOff()
{
    greenState = 0;
    digitalWrite(AlPinGreen, greenState); //ON

}
void Supp::toggleGreen()
{

    greenState = !greenState ;
    digitalWrite(AlPinGreen, greenState); //ON

}

static bool lastNotWD= 0;
void Supp::notWDWakeUp()
{
    if (!lastNotWD)
    {
       // toggleGreen();
    }
    lastNotWD = 1;
}
void Supp::watchdogWakeUp()
{
    lastNotWD = 0;
}

void Supp::powerSaveHigh()
{
    greenOff();
}

void Supp::powerSaveMedium()
{
    greenOn();
}
void Supp::powerSaveLow()
{
    greenOn();
}
