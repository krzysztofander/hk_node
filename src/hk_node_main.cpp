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
#include "comm.h"
#include "supp.h"
#include "temp_measurement.h"
#include "sleeper.h"
#include "temp_sensor.h"
#include "blinker.h"

/*
    Version notes:

    Things to do
    - Update documentation
    - Measure batery level
    - Use EEPROM to store configuration
    -- reset eeprom
    - Improve power saving
    --   Software
    --   HW
    --- remove led
    --- remove USB serial & stuff
    --- make BT on/off
    - Set Bluetooth properlu
    -- configure for given host
    -- reset on reset
    -- turn off led

*/


void setupBody() 
{

    initAllFunctions();
    
}

void loopBody() 
{
    //just woke up
    Supp::mainLoopStart((uint8_t)Sleeper::getUpTime());

    Sleeper::SleepTime timeSlept = Sleeper::howMuchDidWeSleep();
    Executor::adjustToElapsedTime(timeSlept);

    while(
            HKComm::respondSerial()  //loop until returns true
         );  //if this was serial, handle that
    //now execute what needs to be executed...
    ExecutorBase::EExecutors executor = Executor::giveExecutorToCall();
    if (executor < (uint8_t) Executor::executorsNumber)
    {
        Executor::rescheduleExecutor(executor);
        //execute the executor now... 
        ExecutingFn f = Executor::giveExecutorHandleToCall(executor);
        Supp::executorPreAction(executor);
        f();
        Supp::executorPostAction(executor);
    }
    else
    {
        //nothing to call, it might have been from serial
    }
    Sleeper::SleepTime sleepTime = Executor::getNextSleepTime();
        //sleepTime would be 0 if there is more executors to call.

    Sleeper::setNextSleep(sleepTime);

    //go to sleep
    Sleeper::goToSleep(); //if its set to 0 it wont...
    
}

void initAllFunctions(void)
{
    Supp::init();
    Blinker::init();
    TempSensor::init();
        //@todo do something when init is not successfull
    TempMeasure::initMeasureTemperature();
 
    Executor::init();
    Executor::setupExecutingFn((uint8_t)Executor::blinker, 3, Blinker::blinkAction );
        //@todo read that from NV
    Executor::setupExecutingFn((uint8_t)Executor::temperatureMeasurer, 12, TempMeasure::measureTemperature); 
        //@todo read value from NV
    
    Sleeper::init();

}
//---------------------------------------------------------------
