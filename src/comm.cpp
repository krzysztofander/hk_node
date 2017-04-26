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
#include "comm_extra_records.h"
#include "comm_extra_rec_handlers.h"
#include "blinker.h"
#include "nv.h"
#include "string.h"
#include "adc.h"

void HKComm::command_DER(OutBuilder & bld)
{
    bld.reset();
    bld.putCMD(static_cast<uint32_t>(InCommandWrap::ECommands::command_DER));
}
#if HAVE_HUMAN_READABLE
void HKComm::command_AHR(OutBuilder & bld)
{
    bld.putCMD(static_cast<uint32_t>(InCommandWrap::ECommands::command_AHR) );

    bld.addInt(1);
    bld.setHumanReadable();
}
#endif
void HKComm::commandCTP(const InCommandWrap & inCmd, OutBuilder & bld)
{
  
    if (inCmd.hasData())
    {
        OutBuilder::ELogicErr err;
        int32_t newPeriod = (int32_t)inCmd.getInt(err,true,sizeof(int32_t), true);

        if (err != OutBuilder::ELogicErr::None)
        {
            bld.putErr(err);
        }
        else
        {
            Executor::setExecutionTime(
                (uint8_t)Executor::temperatureMeasurer,
                newPeriod);
        }
    }
    bld.putCMD(static_cast<uint32_t>(InCommandWrap::ECommands::command_CTP));
    bld.addInt(Executor::giveExecutionTime((uint8_t)Executor::temperatureMeasurer));
}

void HKComm::commandCBP(const InCommandWrap & inCmd, OutBuilder & bld)
{  //This command is read only

    bld.putCMD(static_cast<uint32_t>(InCommandWrap::ECommands::command_CBP));
    bld.addInt(Executor::giveExecutionTime((uint8_t)Executor::blinker));
}
void HKComm::commandDLS(const InCommandWrap & inCmd, OutBuilder & bld)
{  //This command is read only
    if (inCmd.hasData())
    {
        OutBuilder::ELogicErr err;
        uint8_t newStat = (uint8_t)inCmd.getInt(err,false,sizeof(uint8_t),true);
        if (err != OutBuilder::ELogicErr::None)
        {
            bld.putErr(err);
        }
        Supp::extLEDMasterCtrl((uint8_t)newStat);
    }

    bld.putCMD(static_cast<uint32_t>(InCommandWrap::ECommands::command_DLS));
    bld.addInt(Supp::getExtLEDMasterCtrl());
}



void HKComm::commandCBS(const InCommandWrap & inCmd, OutBuilder & bld)
{  
    if (inCmd.hasData())
    {
        OutBuilder::ELogicErr err;
        uint8_t newPattern =  (uint8_t)inCmd.getInt(err,false,sizeof(uint8_t),true); 

        if (err != OutBuilder::ELogicErr::None)
        {
            bld.putErr(err);
        }
        else
        {
            Blinker::setBlinkPattern(newPattern);
        }
    }
    bld.putCMD(static_cast<uint32_t>(InCommandWrap::ECommands::command_CBS));
    bld.addInt( Blinker::getBlinkPattern());
}
void HKComm::commandCST(const InCommandWrap & inCmd, OutBuilder & bld)
{  
    if (inCmd.hasData())
    {
        OutBuilder::ELogicErr err;
        int64_t newTime = (int64_t)inCmd.getInt(err,true,sizeof(int64_t),true);

        if (err != OutBuilder::ELogicErr::None)
        {
            bld.putErr(err);
        }
        else
        {
            Sleeper::setTime(newTime);
        }
    }
    bld.putCMD(static_cast<uint32_t>(InCommandWrap::ECommands::command_CST));
    bld.addInt( Sleeper::getUpTime());
}
void HKComm::commandCSC(const InCommandWrap & inCmd, OutBuilder & bld)
{
    bld.putCMD(static_cast<uint32_t>(InCommandWrap::ECommands::command_CSC));

    enum SystemCapabilities
    {
        tempMeasurement         = 1U << 0,
        //other external measurement,
        bateryMeasurement       = 1U << 8,
        //other analog measuremet
        //control of output     = 1U << 16
    };

    bld.addInt(tempMeasurement);
}

void HKComm::commandCSM(const InCommandWrap & inCmd, OutBuilder & bld)
{
    if (inCmd.hasData())
    {
        OutBuilder::ELogicErr err;
        uint8_t newPowerMode = (uint8_t)inCmd.getInt(err,false,sizeof(uint8_t),true);

        if (err != OutBuilder::ELogicErr::None)
        {
            bld.putErr(err);
        }
        else
        {
            Sleeper::setPowerSaveMode( Sleeper::PowerSaveMode( newPowerMode) );
        }
    }
    bld.putCMD(static_cast<uint32_t>(InCommandWrap::ECommands::command_CSM));
    bld.addInt(static_cast<uint8_t>(Sleeper::getPowerSaveMode()));
}

void HKComm::commandCRV(const InCommandWrap & inCmd, OutBuilder & bld)
{
    uint16_t bandgapVoltage;

    if (inCmd.hasData())
    {
        OutBuilder::ELogicErr err;
        bandgapVoltage = (uint16_t)inCmd.getInt(err,false,sizeof(uint16_t),true);

        if (err != OutBuilder::ELogicErr::None)
        {
            bld.putErr(err);
        }
        else
        {
            NV::save(NV::NVData::nvBandgapVoltage, &bandgapVoltage);
        }
    }

    bld.putCMD(static_cast<uint32_t>(InCommandWrap::ECommands::command_CRV));

    NV::read(NV::NVData::nvBandgapVoltage, &bandgapVoltage);

    bld.addInt(bandgapVoltage);
}



void HKComm::commandCSA(const InCommandWrap & inCmd, OutBuilder & bld)
{
    if (inCmd.hasData())
    {
        OutBuilder::ELogicErr err;
        uint8_t newPowerDownPeriod = (uint8_t)inCmd.getInt(err,false,sizeof(uint8_t),true);

        if (err != OutBuilder::ELogicErr::None)
        {
            bld.putErr(err);
        }
        else
        {
            Sleeper::setNoPowerDownPeriod( newPowerDownPeriod );
        }
    }
    bld.putCMD(static_cast<uint32_t>(InCommandWrap::ECommands::command_CSA));
    bld.addInt(Sleeper::getNoPowerDownPeriod());

}

void HKComm::commandCNN(const InCommandWrap & inCmd, OutBuilder & bld)
{
    if (inCmd.hasData())
    {
        OutBuilder::ELogicErr err;
        
        const char * newName = inCmd.getString(err);  //null terminated
        if (err != OutBuilder::ELogicErr::None)
        {
            bld.putErr(err);
        }
        else
        {
            NV::save(NV::NVData::nvBTName, newName);
        }
    }
    char buffer[static_cast<int>(NV::NVDataSize::nvBTName)];
    NV::read(NV::NVData::nvBTName, buffer);

    bld.putCMD(static_cast<uint32_t>(InCommandWrap::ECommands::command_CNN));
    bld.addString(buffer, static_cast<uint16_t>(strlen(buffer)) );
    
}

/*

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
*/


//------------------------------------------------------------------
