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


void HKComm::echoLetter(uint8_t l)
{
    uint8_t sequence[] = "0x?? \n\r";
    uint8_t highHex = l >> 4 ;
    highHex += highHex > uint8_t(9) ? uint8_t('a')  - uint8_t(10) : uint8_t('0');
    
    uint8_t lowHex = l & uint8_t(0xF);
    lowHex += lowHex > uint8_t(9) ? uint8_t('a') - uint8_t(10) : uint8_t('0');

    sequence[2] = highHex;
    sequence[3] = lowHex;
    if (l > uint8_t(' '))
    {
        HKSerial::write(&l,1); 
        uint8_t arrow[] = " -> ";
        HKSerial::write(arrow,4); 
        HKSerial::write(sequence, NUM_ELS(sequence)- 3); //no need for EOL
    }
    else
    {
        HKSerial::write(sequence, NUM_ELS(sequence)- 1); //no need for terminating null
    }  
}

//@brief Returns standardize responce for unknown command of given main cathegory
uint8_t HKComm::formatResponceUnkL1(uint8_t (&inOutCommand)[HKCommDefs::commandSize], uint16_t & dataSize)
{
    //inOutCommand[HKCommDefs::commandIdentifierPos] = as is
    inOutCommand[HKCommDefs::command_subIdPos1] = 'u';
    inOutCommand[HKCommDefs::command_subIdPos2] =  'n';
    dataSize = 0;
    return HKCommDefs::serialErr_None;
}

//@brief Returns standardize responce for unknown command of given sub cathegory
uint8_t HKComm::formatResponceUnkL2(uint8_t (&inOutCommand)[HKCommDefs::commandSize], uint16_t & dataSize)
{
    //inOutCommand[HKCommDefs::commandIdentifierPos] = as is
    //inOutCommand[HKCommDefs::command_subIdPos1] = as us
    inOutCommand[HKCommDefs::command_subIdPos2] =  'u';
    dataSize = 0;
    return HKCommDefs::serialErr_None;

}

//@brief Returns standardize responce when command is correct, especially for configure
uint8_t HKComm::formatResponceOK(uint8_t (&inOutCommand)[HKCommDefs::commandSize], uint8_t (&inOutData)[HKCommDefs::commandMaxDataSize], uint16_t & dataSize)
{
    (void)inOutCommand;
    //inOutCommand[HKCommDefs::commandIdentifierPos] = as is
    //inOutCommand[HKCommDefs::command_subIdPos1] = as us
    //inOutCommand[HKCommDefs::command_subIdPos2] = as is
    inOutData[0] = ' ';
    inOutData[1] = 'o';
    inOutData[2] = 'k';
    dataSize = 3;
    return HKCommDefs::serialErr_None;
}




//------------------------------------------------------------------
// @brief Response for D (debug) function  

uint8_t HKComm::command_D(uint8_t (&inOutCommand)[HKCommDefs::commandSize], uint8_t (&inOutData)[HKCommDefs::commandMaxDataSize], uint16_t & dataSize)
{
    uint8_t err = HKCommDefs::serialErr_Assert;
    switch (inOutCommand[HKCommDefs::command_subIdPos1])
    {
    case 'E': //echo 
        //Ignore inOutCommand[command_subIdPos2] 
        //Response:
        //inOutCommand[commandIdentifierPos] = 'D';//already 'D'
          inOutCommand[HKCommDefs::command_subIdPos1] = 'R';
        //inOutCommand[command_subIdPos2] = as is
        err = formatResponceOK(inOutCommand,inOutData, dataSize);
        break;
    case 'L':
        if (inOutCommand[HKCommDefs::command_subIdPos2] == '0')
        {
            Supp::extLEDMasterCtrl(0);
        }
        else
        {
            Supp::extLEDMasterCtrl(1);
        }
        err = formatResponceOK(inOutCommand,inOutData, dataSize);
        break;
    default:  //unknown 'D' command
        err = formatResponceUnkL1(inOutCommand, dataSize);
        break;
    }
    return err;
}

//------------------------------------------------------------------
// @brief Response for C (configuration) request function  
// @Returns 0 ok, 1 or above - serial error

uint8_t HKComm::command_C(uint8_t (&inOutCommand)[HKCommDefs::commandSize], uint8_t (&inOutData)[HKCommDefs::commandMaxDataSize], uint16_t & dataSize)
{
    uint8_t err = HKCommDefs::serialErr_Assert;

    switch (inOutCommand[HKCommDefs::command_subIdPos1])
    {
    case 'T': //configure temperature
        switch (inOutCommand[HKCommDefs::command_subIdPos2])
        {
        case 'M': //CTM
        case 'P':   //Congigure Temperature Period
        {
            Sleeper::SleepTime sleepTime = 0;
            if (dataSize < sizeof(uint16_t) * 2)
            {
                err = HKCommDefs::serialErr_Number_Uint16ToShort;
                break;
            }
            else if (dataSize == sizeof(uint16_t) * 2)
            {
                uint16_t tempMeasmntInterval;
                err = HKCommCommon::dataToUnsignedShort(0, inOutData, tempMeasmntInterval);
                if (err != HKCommDefs::serialErr_None)
                {
                    break;
                }
                sleepTime = (Sleeper::SleepTime)tempMeasmntInterval;
            }
            else if (dataSize == sizeof(int32_t) * 2)
            {
                uint32_t tempMeasmntInterval;
                err = HKCommCommon::dataToUnsigned32(0, inOutData, tempMeasmntInterval);
                if (err != HKCommDefs::serialErr_None)
                {
                    break;
                }
                sleepTime = (Sleeper::SleepTime)tempMeasmntInterval;
            }
            else
            {
                err = HKCommDefs::serialErr_Number_NoCorrectLength;
                break;
            }
            Executor::setExecutionTime((uint8_t)Executor::temperatureMeasurer, sleepTime);
            err = formatResponceOK(inOutCommand,inOutData, dataSize);
          
        }
        break;
        default:
            err = formatResponceUnkL2(inOutCommand, dataSize);
             break;
        }
        break;
    case 'S' :
        switch (inOutCommand[HKCommDefs::command_subIdPos2])
        {
            case 'T':
            {  //configire system time
                uint64_t newTime;
                err = HKCommCommon::dataToUnsigned64(0, inOutData, newTime);
                if (err != HKCommDefs::serialErr_None)
                {
                    break;
                }
                Sleeper::setTime(HKTime::UpTime(newTime));
                err = formatResponceOK(inOutCommand, inOutData, dataSize);
            }
            break;
            default:
                err = formatResponceUnkL2(inOutCommand, dataSize);
                break;
        }
        break;
    case 'P' : //power saving mode
        switch (inOutCommand[HKCommDefs::command_subIdPos2])
        {
            case 'M': //mode
            {
                uint8_t PsMode;
                err = HKCommCommon::dataToUnsigned8(0, inOutData, PsMode);
                if (err != HKCommDefs::serialErr_None)
                {
                    break;
                }
                Sleeper::setPowerSaveMode((Sleeper::PowerSaveMode)PsMode);
                err = formatResponceOK(inOutCommand, inOutData, dataSize);
            }
            break;
            case 'A': //power awake time after non-wd wake up
            {
                uint8_t PMAwake;
                err = HKCommCommon::dataToUnsigned8(0, inOutData, PMAwake);
                if (err != HKCommDefs::serialErr_None)
                {
                    break;
                }
                Sleeper::setNoPowerDownPeriod(PMAwake);
                err = formatResponceOK(inOutCommand, inOutData, dataSize);
            }
            break;
            default:
                err = formatResponceUnkL2(inOutCommand, dataSize);
                break;
        }
        break;
    case 'B' : //power saving mode
        switch (inOutCommand[HKCommDefs::command_subIdPos2])
        {
            case 'P': //mode
            {
                uint8_t pattern;
                err = HKCommCommon::dataToUnsigned8(0, inOutData, pattern);
                if (err != HKCommDefs::serialErr_None)
                {
                    break;
                }
                Blinker::setBlinkPattern(pattern);
                err = formatResponceOK(inOutCommand, inOutData, dataSize);
            }
            break;
            default:
                err = formatResponceUnkL2(inOutCommand, dataSize);
                break;
        }
        break;
    default:  //unknown 'C' command
        //Response:
        err = formatResponceUnkL1(inOutCommand, dataSize);
        break;
    }
    return err;
}


//------------------------------------------------------------------
