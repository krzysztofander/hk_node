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

#include "executor.h"
#include "hk_node.h"
#include "supp.h"

Sleeper::SleepTime Executor::g_ExecutorsTimeLeft[Executor::executorsNumber];  // how much time has left to next call
Sleeper::SleepTime Executor::g_ExecutorsPeriods[Executor::executorsNumber];  //how often each exec is called
Executor::Executor::ExecutingFn Executor::g_ExecutingFunctions[Executor::executorsNumber];

uint8_t Executor::isExecutorActive(uint8_t executor)
{
    return  (executor < executorsNumber && g_ExecutingFunctions[executor] !=  0) ? 1 : 0;
}

void Executor::setupExecutingFn(uint8_t executor, Sleeper::SleepTime defaultTime, Executor::ExecutingFn f)
{
    if (executor < (uint8_t)executorsNumber)
    {
        g_ExecutingFunctions[executor] = f;
        g_ExecutorsPeriods[executor] = defaultTime;
        g_ExecutorsTimeLeft[executor] = 0; // expected to be called immediately after setup
    }
    else
    {
        Supp::dbgAlert(Supp::AlertReason_BadParam, true);
    }

}


void Executor::init (void)
{
    for (uint8_t i = 0; i < (uint8_t)executorsNumber; i++)
    {
        g_ExecutorsTimeLeft[i] = 0;
        g_ExecutorsPeriods[i] = 0;
        g_ExecutingFunctions[i] = 0;
    }
}

//@Brief Returns a executor to call now
// It is expected that ones executor was called it gets rescheduled
ExecutorBase::EExecutors Executor::giveExecutorToCall(void)
{
    for (uint8_t i = 0; i < (uint8_t)executorsNumber; i++)
    {
        if (isExecutorActive (i) && g_ExecutorsTimeLeft[i] == 0 )
        {
            return (ExecutorBase::EExecutors)i;
        }
    }
    return executorNone;
}

Executor::ExecutingFn Executor::giveExecutorHandleToCall(ExecutorBase::EExecutors executor)
{
    if (isExecutorActive (executor)  )
    {
        return g_ExecutingFunctions[executor];
    }
    else
    {
        Supp::dbgAlert(Supp::AlertReason_BadParam, true);
        return 0;
    }
}



Sleeper::SleepTime Executor::getNextSleepTime(void)
{
    //returns the shortest time remaining for all ACTIVE executors
    //or 0 if nothing left
    Sleeper::SleepTime nextTime = Sleeper::SleepTime(0x7FFFFFFF);
         
    for (uint8_t i = 0; i < (uint8_t)executorsNumber; i++)
    {
        if (isExecutorActive(i) && g_ExecutorsTimeLeft[i] < nextTime)
        {
            nextTime = g_ExecutorsTimeLeft[i];
        }
    }
    return nextTime;
}
//@Brief Decreases the table of execution times by the time passed
void Executor::adjustToElapsedTime(Sleeper::SleepTime timePassed)
{
    //called when woken up. Need to substract time passed from g_ExecutorsTimeLeft
    //so can now see what to execute and how much has left
    for (uint8_t i = 0; i < (uint8_t)executorsNumber; i++)
    {
        if (isExecutorActive(i))
        {
            //executor is active....
            if (g_ExecutorsTimeLeft[i] < timePassed)
            {
                //this should not have happened unless time passed while doing other executor
                //routine.
                g_ExecutorsTimeLeft[i] = 0;
            }
            else
            {
                g_ExecutorsTimeLeft[i] -= timePassed;
            }
        }
    }

}

void Executor::setExecutionTime(uint8_t executorToSet, Sleeper::SleepTime newTime)
{
    if (isExecutorActive(executorToSet) )
    {
        g_ExecutorsPeriods[executorToSet] = newTime;
        //if new time is shorter than remaining adjust remaining

        //if new time is longer than remaining  adjust remaining as well
        g_ExecutorsTimeLeft[executorToSet] = newTime;
    }
    else
    {
        Supp::dbgAlert(Supp::AlertReason_BadParam, true);
    }
}

//@brief Reads time given to particular executor
//@returns value or -1 if particular executor is not active
Sleeper::SleepTime  Executor::giveExecutionTime(uint8_t executorToRead)
{
    if (isExecutorActive(executorToRead))
    {
        return g_ExecutorsPeriods[executorToRead];
    }
    else
    {
        return -1;
    }

}

void Executor::rescheduleExecutor(uint8_t executor)
{
    if (isExecutorActive(executor))
    {
         g_ExecutorsTimeLeft[executor] =  g_ExecutorsPeriods[executor] ;
    }
    else
    {
        Supp::dbgAlert(Supp::AlertReason_BadParam, true);
    }
}
