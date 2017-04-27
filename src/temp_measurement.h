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

#ifndef HK_TEMP_MEASUREMENT_H
#define HK_TEMP_MEASUREMENT_H
#include "hk_node.h"

//--------------------------------------------------

class TempMeasure
{
public:
    
    typedef int16_t TempMeasurement;  //change the tempMeasurement_invalid if changing this type

    static const int8_t maxMeasurements = 64;
    static const int16_t tempMeasurement_invalid = -0x8000;  

    struct TempRecord
    {
        TempRecord()
            : timeStamp(0)
            , tempFPCelcjus(TempMeasure::tempMeasurement_invalid)
        {}


        TempRecord(HKTime::SmallUpTime timeStamp, TempMeasurement tempFPCelcjus)
            : timeStamp(timeStamp)
            , tempFPCelcjus(tempFPCelcjus)
        {}

        HKTime::SmallUpTime    timeStamp;
        TempMeasurement        tempFPCelcjus;
    };

    static TempMeasurement getSingleTempMeasurement(void);                     
    static void getSingleTempMeasurement(TempRecord & out, const HKTime::SmallUpTime currentTime);
    static TempRecord getTempMeasurementRecord(uint16_t howManyRecordsBack);
    static int16_t capacity()
    {
        return maxMeasurements;
    }


    static void initMeasureTemperature(void);                //init
    static void measureTemperature(void);                    //measure no

public:
    static TempRecord  g_tempMeasurements[maxMeasurements];

    static uint16_t g_lastTempMeasurementIt;
};
//-------------------------------------------------
#endif

