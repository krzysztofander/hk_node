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
#include "executor.h"
#include "serial.h"
#include "temp_measurement.h"




void setupBody() 
{
    // test
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(AlPin1, OUTPUT);
    pinMode(AlPin2, OUTPUT);
    pinMode(AlPin3, OUTPUT);
    pinMode(AlPin4, OUTPUT);
    pinMode(AlPinBlue, OUTPUT);

    
    pinMode(buttonPin, INPUT);
    
    for (int i = 0; i < 10; i++)
    {
        toggleBlue();
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



//---------------------------------------------------------------
void ledToggler(void);
void initAllFunctions(void)
{
    initMeasureTemperature();
    Executor::init();

    Executor::setupExecutingFn((uint8_t)Executor::blinker, 6, ledToggler);
    Sleeper::initWD();
   
}
//---------------------------------------------------------------
