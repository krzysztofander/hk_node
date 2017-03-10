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
#include "comm_common.h"
#include "comm.h"
//------------------------------------------------------------------

volatile static uint8_t gv_blueState = 0;

void ledToggler(void)
{
    for (int i = 0; i < 3; i++)
    {
        digitalWrite(LED_BUILTIN, 1);
        delay(20);
        digitalWrite(LED_BUILTIN, 0);
        delay(40);
    }
    uint8_t buttonState;
    buttonState = digitalRead(buttonPin);
    if (!buttonState)
    {
        static int8_t couter = 0;

        uint16_t dataSize = 0;
        HKComm::g_data[dataSize++] = 'A';
        HKComm::g_data[dataSize++] = 'H';
        HKComm::g_data[dataSize++] = 'L';
        HKCommCommon::uint8ToData(dataSize, HKComm::g_data, couter ++);
        
        Serial.write(HKComm::g_data,dataSize);
        Serial.write(HKCommDefs::commandEOLOnResponceSequence,NUM_ELS(HKCommDefs::commandEOLOnResponceSequence) );
    };


}


void toggleBlue(void)
{
    digitalWrite(AlPinBlue, gv_blueState);
    gv_blueState = !gv_blueState;
}

void blueOn(void)
{
    digitalWrite(AlPinBlue, 0);
    gv_blueState = 0;
}

void blueOff(void)
{
    digitalWrite(AlPinBlue, 1);
    gv_blueState = 1;
}

void blinkBlue(void)
{
  for (uint8_t i =0; i < 5; i++)
  {
    toggleBlue();
    delay(20);
  }
  blueOff();
}

void alert(register AlertReason reason, bool hold)
{
      switch (reason)
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
        0 && // disaled for now
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
}
//---------------------------------------------------------------
