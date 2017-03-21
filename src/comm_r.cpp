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


#include "hk_node.h"
#include "executor.h"
#include "serial.h"
#include "comm.h"
#include "supp.h"
#include "temp_measurement.h"
#include "comm_common.h"
#include "comm_extra_records.h"
#include "comm_extra_rec_handlers.h"
#include "blinker.h"


//@brief Read Version Information
//Returns version of this software
uint8_t HKComm::command_RVI(uint8_t (&inOutCommand)[HKCommDefs::commandSize], uint8_t (&inOutData)[HKCommDefs::commandMaxDataSize], uint16_t & dataSize)
{
    dataSize = 0;
    inOutCommand[HKCommDefs::commandIdentifierPos] =  'V';
    inOutData[dataSize++] = ' ';
    inOutData[dataSize++] = '0';
    inOutData[dataSize++] = '.';
    inOutData[dataSize++] = '5';
    inOutData[dataSize++] = '.';
    inOutData[dataSize++] = '1';
    inOutData[dataSize++] = '.';
    inOutData[dataSize++] = '4';
// Datasheet: http://www.atmel.com/Images/Atmel-42735-8-bit-AVR-Microcontroller-ATmega328-328P_Datasheet.pdf


    /*Releases
    0.5.0:
        + version information
        + shortened RTH
        + sending temperature readings set up by blinker
    
    0.5.1 Development
    0.5.1.1 Development
        + Improved current power saving algo. 
        + Starting serial now in main loop after non-wd power up 
          and waiting a delay afterwards
    0.5.1.2 Development
        +Improved blinker setting
    0.5.1.3 Development
        +Improved blinker setting, fixed auto send
    0.5.1.4 Development
        +Added compatible commands

    0.5.2 (planned)
        improved power save mode
        //see http://www.home-automation-community.com/arduino-low-power-how-to-run-atmega328p-for-a-year-on-coin-cell-battery/

    0.?.0 (planned)
        + batery reading
            // see https://forum.arduino.cc/index.php?topic=38119.0
    0.?.1
        + Any AC value reading with autoscale
    0.?.1
        + Bluetooth settings
    0.?.1 
        + eeprom
    1.0.0 all above works



    */
    return HKCommDefs::serialErr_None;
}


uint8_t HKComm::command_RTM(uint8_t (&inOutCommand)[HKCommDefs::commandSize], uint8_t (&inOutData)[HKCommDefs::commandMaxDataSize], uint16_t & dataSize)
{
    TempMeasure::TempMeasurement singleTempMeasurement = TempMeasure::getSingleTempMeasurement();
    dataSize = 0;
    inOutCommand[HKCommDefs::commandIdentifierPos] =  'V';
 
    HKCommExtraRecordsHDL::setNumRecords(0);
    HKCommExtraRecordsHDL::setDataReciever(&HKCommExtraHLRs::RTHdataReciever);
    
    return  HKCommCommon::formatMeasurement(dataSize, inOutData, HKTime::SmallUpTime (Sleeper::getUpTime()), singleTempMeasurement);
    
}

uint8_t HKComm::command_RTH(uint8_t (&inOutCommand)[HKCommDefs::commandSize], uint8_t (&inOutData)[HKCommDefs::commandMaxDataSize], uint16_t & dataSize)
{
    uint8_t err;
    //fake for quick query
    if (dataSize == 0)
    {
        inOutData[0] = '0';
        inOutData[1] = '0';
        inOutData[2] = '1';
        inOutData[3] = '0';
        dataSize = 4;
    }

    //check for size correctness
    if (dataSize < sizeof(short) * 2)
    {
        err = HKCommDefs::serialErr_Number_Uint16ToShort;
        return err;
    }
    uint16_t measurementsToReturn;
    err  = HKCommCommon::dataToUnsignedShort(0, inOutData, measurementsToReturn);
    if (err != HKCommDefs::serialErr_None)
    {
        return err;
    }
    //make it sane
    if (measurementsToReturn == 0 || measurementsToReturn > TempMeasure::capacity())
    { //it its zero return all.
        measurementsToReturn = TempMeasure::capacity();
    }

    dataSize = 0;
    inOutCommand[HKCommDefs::commandIdentifierPos] =  'V';

    //measurementsToReturn contains how many. First one returns difference of current to timestamp
    HKTime::UpTime diff = Sleeper::getUpTime();
    TempMeasure::TempRecord tempRecord = TempMeasure::getTempMeasurementRecord(0);
    diff = diff - (HKTime::UpTime)tempRecord.timeStamp;
    err = HKCommCommon::formatMeasurement(dataSize,
                                          inOutData,
                                          HKTime::SmallUpTime(diff),
                                          tempRecord.tempFPCelcjus);

    if (err != HKCommDefs::serialErr_None)
    {
        return err;
    }

    measurementsToReturn--; //one is returned in inOutData

                            //now set up records handler
    if (measurementsToReturn > 0)
    {
        HKCommExtraRecordsHDL::setNumRecords(measurementsToReturn);
        HKCommExtraRecordsHDL::setDataReciever(&HKCommExtraHLRs::RTHdataReciever);
    }
    return err;
}

uint8_t HKComm::command_RTP(uint8_t (&inOutCommand)[HKCommDefs::commandSize], uint8_t (&inOutData)[HKCommDefs::commandMaxDataSize], uint16_t & dataSize)
{
    inOutCommand[HKCommDefs::commandIdentifierPos] = 'V';
    Sleeper::SleepTime tempMeasurementTime = Executor::giveExecutionTime((uint8_t)Executor::temperatureMeasurer);
    dataSize = 0;
    return HKCommCommon::uint32ToData(dataSize, inOutData, (uint32_t)tempMeasurementTime);
}


uint8_t HKComm::command_RST(uint8_t (&inOutCommand)[HKCommDefs::commandSize], uint8_t (&inOutData)[HKCommDefs::commandMaxDataSize], uint16_t & dataSize)
{
    inOutCommand[HKCommDefs::commandIdentifierPos] = 'V';
    return HKCommCommon::uint64ToData(dataSize, inOutData, Sleeper::getUpTime());
}

uint8_t HKComm::command_RBP(uint8_t (&inOutCommand)[HKCommDefs::commandSize], uint8_t (&inOutData)[HKCommDefs::commandMaxDataSize], uint16_t & dataSize)
{
    inOutCommand[HKCommDefs::commandIdentifierPos] = 'V';
    Sleeper::SleepTime blinkerPediod = Executor::giveExecutionTime((uint8_t)Executor::blinker);
    dataSize = 0;
    return HKCommCommon::uint32ToData(dataSize, inOutData, (uint32_t)blinkerPediod);
}
uint8_t HKComm::command_RBA(uint8_t (&inOutCommand)[HKCommDefs::commandSize], uint8_t (&inOutData)[HKCommDefs::commandMaxDataSize], uint16_t & dataSize)
{
    inOutCommand[HKCommDefs::commandIdentifierPos] = 'V';
    return HKCommCommon::uint8ToData(dataSize, inOutData, Blinker::getBlinkPattern());
}

uint8_t HKComm::command_RPM(uint8_t (&inOutCommand)[HKCommDefs::commandSize], uint8_t (&inOutData)[HKCommDefs::commandMaxDataSize], uint16_t & dataSize)
{
    inOutCommand[HKCommDefs::commandIdentifierPos] = 'V';
    return HKCommCommon::uint8ToData(dataSize, inOutData, Sleeper::getPowerSaveMode());
}
uint8_t HKComm::command_RPA(uint8_t (&inOutCommand)[HKCommDefs::commandSize], uint8_t (&inOutData)[HKCommDefs::commandMaxDataSize], uint16_t & dataSize)
{
    inOutCommand[HKCommDefs::commandIdentifierPos] = 'V';
    return HKCommCommon::uint8ToData(dataSize, inOutData, Sleeper::getNoPowerDownPeriod());
}

//https://forum.arduino.cc/index.php?topic=38119.0


uint8_t HKComm::command_R(uint8_t (&inOutCommand)[HKCommDefs::commandSize], uint8_t (&inOutData)[HKCommDefs::commandMaxDataSize], uint16_t & dataSize)
{
    uint8_t err = HKCommDefs::serialErr_Assert;
    

    switch (inOutCommand[HKCommDefs::command_subIdPos1])
    {
        case 'T': //temperature measurer
            switch (inOutCommand[HKCommDefs::command_subIdPos2])
            {
                case 'M':   //make a measurement
                    err = command_RTM(inOutCommand, inOutData, dataSize);
                    break;
                case 'R':
                case 'H':  //temperatue history
                    err =  command_RTH(inOutCommand, inOutData, dataSize);
                    break;
                case 'C':  //temperatue configuration (deprecated)
                case 'P':  //R/V temperature period
                    err =  command_RTP(inOutCommand, inOutData, dataSize);
                    break;
                default:
                    err = formatResponceUnkL2(inOutCommand, dataSize);
                    break;
            }
            break;
        case 'B': //blinker
            switch (inOutCommand[HKCommDefs::command_subIdPos2])
            {
                case 'P':
                    err = command_RBP(inOutCommand, inOutData, dataSize);
                    break;
                case 'A':
                    err = command_RBA(inOutCommand, inOutData, dataSize);
                    break;
                default:
                    err = formatResponceUnkL2(inOutCommand, dataSize);
            }
            break;
        case 'S':
            switch (inOutCommand[HKCommDefs::command_subIdPos2])
            {
                case 'T':
                    //read system time
                    err = command_RST(inOutCommand, inOutData, dataSize);
                    break;
                default:
                    err = formatResponceUnkL2(inOutCommand, dataSize);
                    break;

            }
            break;
        case 'P':
            switch (inOutCommand[HKCommDefs::command_subIdPos2])
            {
                case 'M':
                    err = command_RPM(inOutCommand, inOutData, dataSize);
                    break;
                case 'A':
                    err = command_RPA(inOutCommand, inOutData, dataSize);
                    break;
                default:
                    err = formatResponceUnkL2(inOutCommand, dataSize);
                    break;

            }
            break;
        case 'V':  //Read Version
            err = command_RVI(inOutCommand, inOutData, dataSize);
            break;
        default:  //unknown 'R' command
            err = formatResponceUnkL1(inOutCommand, dataSize);
            break;

    }
    return err;  
}

//------------------------------------------------------------------

