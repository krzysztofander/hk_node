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
//------------------------------------------------------------------

void toggleBlue(void)
{
    volatile static uint8_t state = 0;
    digitalWrite(AlPinBlue, state);
    state = !state;
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
       if (hold)
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
