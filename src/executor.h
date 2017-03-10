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
#ifndef HK_EXECUTORS_H
#define HK_EXECUTORS_H

#include <Arduino.h>
#include "hk_node.h"
#include "sleeper.h"

class Executor
{
public:

    enum EExecutors
    {
        blinker = 0,
        temperatureMeasurer,
        fakeExecutor1,
        fakeExecutor2,
        fakeExecutor3,

        executorsNumber,

        executorNone = 255

    };
    static void init (void);

    static void adjustToElapsedTime(Sleeper::SleepTime timePassed);
    static uint8_t giveExecutorToCall(void);
    
    static Sleeper::SleepTime getNextSleepTime(void);
    static void setExecutionTime(uint8_t executorToSet, Sleeper::SleepTime newTime);
    static Sleeper::SleepTime  giveExecutionTime(uint8_t executorToRead);
    static void rescheduleExecutor(uint8_t executor);
    static void setupExecutingFn(uint8_t executor, Sleeper::SleepTime defaultTime,  ExecutingFn f );
    static ExecutingFn giveExecutorHandleToCall(uint8_t executor);
    static uint8_t isExecutorActive(uint8_t executor);

public:
    static Sleeper::SleepTime g_ExecutorsTimeLeft[executorsNumber];  // how much time has left to next call
    static Sleeper::SleepTime g_ExecutorsPeriods[executorsNumber];  //how ofthen each exec is called
    static ExecutingFn g_ExecutingFunctions[executorsNumber];
};

#endif
