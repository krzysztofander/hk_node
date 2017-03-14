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

#ifndef HK_SUPP_H
#define HK_SUPP_H

//--------------------------------------------------
#if 0
typedef  uint8_t AlertReason;

enum EAlertReasons
{
    AlertReason_Step1 = 1,
    AlertReason_Step2 = 2,
    AlertReason_Step3 = 3,
    
    AlertReason_serialSend  = 4,
    AlertReason_serialuint8_t  = 5,
    AlertReason_serialwriteProblem = 6,
    AlertReason_serialReadProblem = 7,
    AlertReason_intervalSet = 8,
    
    AlertReason_PassedOverTime = 9,
    AlertReason_BadParam = 10,

    AlertReason_ExecutorCalled = 11,
    AlertReason_serialChar = 12


};

void alert(register AlertReason reason, bool hold);
void toggleBlue(void);
void blueOff(void);
void blueOn(void);
void blinkBlue(void);
void ledToggler(void);
#endif
#include "executor_base.h"

void ledToggler(void);

class Supp
{
public:
    //@ Initalizes Support class. Reads the platform type from pins, if any
    static void init();
    
    enum PlatformType
    {
        TempMeasurer

    };
    //@returns Platform type
    static PlatformType getPlatformType(); 
    
    enum EAlertReasons
    {
        AlertReason_Step1 = 1,
        AlertReason_Step2 = 2,
        AlertReason_Step3 = 3,

        AlertReason_serialSend  = 4,
        AlertReason_serialuint8_t  = 5,
        AlertReason_serialwriteProblem = 6,
        AlertReason_serialReadProblem = 7,
        AlertReason_intervalSet = 8,

        AlertReason_PassedOverTime = 9,
        AlertReason_BadParam = 10,

        AlertReason_ExecutorCalled = 11,
        AlertReason_serialChar = 12


    };
    //@debug function, can do whatever
    static void dbgAlert(uint8_t alertVal, bool hold);
   
    //@ Reacts on main loop loops and on passing time
    static void mainLoopStart(uint8_t timeMod256);

    //@ Power down indication
    static void aboutToPowerDown();

    //@ Power up indication
    static void justPoweredUp();

    //@ No Powerdown happened
    static void noPowerDownHappened();

    //@ Reacts of action from the executor
    static void executorPreAction(ExecutorBase::EExecutors eexecutor);
    static void executorPostAction(ExecutorBase::EExecutors eexecutor);


    //@ build in LED blinker
    //@param pattern, a blink pattern 1-short blink, 0 period as long as there is  
    // at least one more MSB bit set as 1
    static void blinkLed(uint8_t pattern); 


    //@a button state
    static bool getButtonState();
private:
    //@Pin assignment
    //this will depend on a platform I guess.
    
    enum EAlertPins
    {
        AlPinBlue = 9,
        AlPin1 = 8,
        AlPin2 = 7,
        AlPin3 = 6,
        AlPin4 = 5,
        PowerUpDown = 8,
        buttonPin = 3



    };
    static uint8_t prevTimeMod256;
};


//--------------------------------------------------



#endif

