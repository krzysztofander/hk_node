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

#ifndef HK_NODE_H
#define HK_NODE_H

#define NUM_ELS(tab) (sizeof(tab)/sizeof(tab[0]))

void setupBody();
void loopBody();

//--------------------------------------------------
typedef  unsigned char WakeUpReason;
#define WakeUpReason_timeout ((WakeUpReason)1)
#define WakeUpReason_serial  ((WakeUpReason)2)
WakeUpReason getWKReason(void);
//--------------------------------------------------
typedef  unsigned char AlertReason;
enum EAlertPins
{
   AlPin1 = 8,
   AlPin2 = 7,
   AlPin3 = 6,
   AlPin4 = 5,

   buttonPin = 3
};
enum EAlertReasons
{
    AlertReason_unknownWakeUp = 8,
    AlertReason_serialSend  = 1,
    AlertReason_serialChar  = 2
};

void alert(register AlertReason reason, bool hold);
//--------------------------------------------------
typedef void (*DoWhatYouShould)(void);
#define MaxDoWhatYouShould 8
extern DoWhatYouShould g_doWhatYouShouldTab[MaxDoWhatYouShould];
void initAllFunctions(void);
void setupDoWhatYouShouldTab(void);
void doWhatYouShould(void);

//--------------------------------------------------
typedef signed short TempMeasurement;

TempMeasurement getSingleTempMeasurement(void);
void initMeasureTemperature(void);

void measureTemperature(void);
#define maxMeasurements  64
#define TempMeasurement_invalid (((TempMeasurement )1) << ((sizeof(TempMeasurement) * 8) -1))
extern TempMeasurement g_tempMeasurements[maxMeasurements];
extern unsigned short g_lastTempMeasurementIt;
//-------------------------------------------------

void respondSerial(void);


//--------------------------------------------------
typedef unsigned short SleepTime;
SleepTime getRemainingSleepTime(void);
SleepTime getDefaultSleepTime(void);
void setNextSleep(SleepTime  st);

//--------------------------------------------------



#endif

