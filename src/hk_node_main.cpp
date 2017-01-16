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
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/io.h>
#include <avr/wdt.h>


#include "hk_node.h"
#include "executor.h"
#include "serial.h"



void setupBody() 
{
    // test
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(AlPin1, OUTPUT);
    pinMode(AlPin2, OUTPUT);
    pinMode(AlPin3, OUTPUT);
    pinMode(AlPin4, OUTPUT);
    pinMode(buttonPin, INPUT);
    
    for (int i = 0; i < 10; i++)
    {
        digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(20);                       // wait for a second
        digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
        delay(20);                       // wait for a second
    }
    alert(14, true);
    
    //end of test
    Serial.println("hello world");

    alert(0, false);
    initAllFunctions();

    
}

void loopBody() 
{
    //just woke up

    //alert(AlertReason_Step1, false);
    uint16_t timeSlept = Sleeper::howMuchDidWeSleep();
    Executor::adjustToElapsedTime(timeSlept);

    while(HKComm::respondSerial());  //if this was serial, handle that
   if (1)
    
{
   // alert(AlertReason_Step2, true);
    //now execute what needs to be executed...
    uint8_t executor = Executor::giveExecutorToCall();
   // alert(AlertReason_Step3, true);
    if (executor < (uint8_t) Executor::executorsNumber)
    {
        //alert(AlertReason_Step2, true);
        Executor::rescheduleExecutor(executor);
        //execute the executor now... 
        //alert(AlertReason_Step3, true);

        ExecutingFn f = Executor::giveExecutorHandleToCall(executor);
        //alert(AlertReason_Step2, true);

        f();
        //alert(AlertReason_Step3, true);
    }
    else
    {
        //nothing to call, it might have been from serial
        //  alert(0, false);
    }
    //alert(AlertReason_Step2, false);
    uint16_t sleepTime = Executor::getNextSleepTime();
    if (0 && sleepTime < 15)
    {
       //alert (sleepTime, false);
    }
    Sleeper::setNextSleep(sleepTime);

    //go to sleep
    Sleeper::gotToSleep(); //if its set to 0 it wont...
}
}
//------------------------------------------------------------------


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
void ledToggler(void);
void initAllFunctions(void)
{
    initMeasureTemperature();
    Executor::init();

    Executor::setupExecutingFn((uint8_t)Executor::blinker, 6, ledToggler);
   
   
}

//---------------------------------------------------------------
Sleeper::SleepTime Sleeper::g_sleepTime = 0;
void Sleeper::setNextSleep(Sleeper::SleepTime  st)
{
    g_sleepTime = st;
}
Sleeper::SleepTime Sleeper::howMuchDidWeSleep(void)
{
    return 1; //todo Fix THAT
}
//http://www.gammon.com.au/forum/?id=11497

// watchdog interrupt
ISR (WDT_vect) 
{
   wdt_disable();  // disable watchdog
}  // end of WDT_vect


void Sleeper::gotToSleep(void)
{
  delay (1000);
 if (0 && g_sleepTime > 0)
  {
   digitalWrite(LED_BUILTIN, 0);
  // disable ADC
    ADCSRA = 0;  
  
    // clear various "reset" flags
    MCUSR = 0;     
    noInterrupts ();  
    // allow changes, disable reset
    WDTCSR = bit (WDCE) | bit (WDE);
    // set interrupt mode and an interval 
    //WDTCSR = bit (WDIE) | bit (WDP3) | bit (WDP0);    // set WDIE, and 8 seconds delay
    WDTCSR = bit (WDIE) | bit (WDP2) | bit (WDP1) ;    // set WDIE, and 1 seconds delay

    interrupts ();    
    
    wdt_reset();  // pat the dog
  
    //set_sleep_mode (SLEEP_MODE_PWR_SAVE);  
    set_sleep_mode(SLEEP_MODE_IDLE);
    noInterrupts ();           // timed sequence follows
      sleep_enable();
      // turn off brown-out enable in software
      //MCUCR = bit (BODS) | bit (BODSE);
      //MCUCR = bit (BODS); 
    interrupts ();             // guarantees next instruction executed
    sleep_cpu ();  
  
    // cancel sleep as a precaution
    sleep_disable();
    digitalWrite(LED_BUILTIN, 1);
  }
  
   // if (g_sleepTime > 0)
   if (0)
    {
        // Choose our preferred sleep mode:
        set_sleep_mode(SLEEP_MODE_IDLE);
     
        // Set sleep enable (SE) bit:
        sleep_enable();
        digitalWrite(LED_BUILTIN, 0);
        // Put the device to sleep:
        sleep_mode();
     
        // Upon waking up, sketch continues from this point.
        digitalWrite(LED_BUILTIN, 1);
        sleep_disable();

        delay(80);

        //delay(100 * g_sleepTime);
    }
}


