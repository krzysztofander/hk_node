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
#include "temp_measurement.h"
#include "sleeper.h"
#include "supp.h"
#include "temp_sensor.h"
//------------------------------------------------------------------
TempMeasure::TempRecord TempMeasure::g_tempMeasurements[TempMeasure::maxMeasurements];
uint16_t TempMeasure::g_lastTempMeasurementIt =  NUM_ELS(g_tempMeasurements)  -1 ;   //points to the last by purpose
HKTime::UpTime TempMeasure::g_lastMeasurementTime = 0; 

//@brief Inits the records table and iterator
void TempMeasure::initMeasureTemperature(void)
{
    for (uint16_t i = 0 ; i < NUM_ELS(g_tempMeasurements); i++)
    {
        g_tempMeasurements[i].tempFPCelcjus = TempMeasure::tempMeasurement_invalid;
        g_tempMeasurements[i].timePassed = 0;
    }
    g_lastTempMeasurementIt =  NUM_ELS(TempMeasure::g_tempMeasurements) - 1;
    TempMeasure::g_lastMeasurementTime = Sleeper::getUpTime();
}

//@brief Makes a single temperature measurement and returns temperature in celcius in fixed point
TempMeasure::TempMeasurement TempMeasure::getSingleTempMeasurement(void)
{
    float reading =  TempSensor::readTemperature();
    uint16_t X = uint16_t(reading * 16);
    blinkBlue();
    
    return X;
}

//@brief Makes a temperature measurement and constucts the temperature measurement record
void TempMeasure::getSingleTempMeasurement(TempRecord & out, const HKTime::UpTime & currentTime, const HKTime::UpTime & lastUpTime)
{
    out.timePassed = HKTime::getShortDiff(currentTime, lastUpTime);
    out.tempFPCelcjus = getSingleTempMeasurement();
}

//@brief Makes a temperature measurement store measurement record
void TempMeasure::measureTemperature(void)
{
    uint16_t lastMeasurementIt = TempMeasure::g_lastTempMeasurementIt;
    g_lastTempMeasurementIt++;

    if (g_lastTempMeasurementIt >= NUM_ELS(g_tempMeasurements)  )
    {
        g_lastTempMeasurementIt  = 0;
    }
    HKTime::UpTime thisUpTime = Sleeper::getUpTime();
    getSingleTempMeasurement(g_tempMeasurements[g_lastTempMeasurementIt], thisUpTime , g_lastMeasurementTime);

    g_lastMeasurementTime = thisUpTime;
}

//@brief returs temp measurement record back in records number
TempMeasure::TempRecord TempMeasure::getTempMeasurementRecord(uint16_t howManyRecordsBack)
{
    if (howManyRecordsBack > NUM_ELS(g_tempMeasurements) - 1)
    {
        howManyRecordsBack = NUM_ELS(g_tempMeasurements) - 1;
    }
    uint16_t it = g_lastTempMeasurementIt + NUM_ELS(g_tempMeasurements) - howManyRecordsBack;
    while (it >= NUM_ELS(g_tempMeasurements))
    {
        it -= NUM_ELS(g_tempMeasurements);
    }
    return g_tempMeasurements[it];
}




