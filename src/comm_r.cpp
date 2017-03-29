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

void HKComm::command_RTM(OutBuilder & bld)
{

    TempMeasure::TempMeasurement singleTempMeasurement = TempMeasure::getSingleTempMeasurement();

    HKCommExtraRecordsHDL::setNumRecords(0);
    HKCommExtraRecordsHDL::setDataReciever(&HKCommExtraHLRs::RTHdataReciever);

    bld.putCMD(static_cast<uint32_t>(InCommandWrap::ECommands::command_VTM));

    bld.addMeasurement(0, singleTempMeasurement);

}


void HKComm::command_RTH(const InCommandWrap & inCmd, OutBuilder & bld)
{
    OutBuilder::ELogicErr err;
    uint16_t measurementsToReturn = g_RecievedCmd.getUint16(err);
    if (err != OutBuilder::ELogicErr::None )
    {
        bld.putErr(err);
    }
    else
    {
        if (measurementsToReturn == 0)
        {
            //special case, make a measurement now and store it.
            TempMeasure::measureTemperature();
            measurementsToReturn = 1; //so we return the last one.
        }

    //check for size correctness
        if (measurementsToReturn > TempMeasure::capacity())
        {
            //if its zero return all.
            measurementsToReturn = TempMeasure::capacity();
        }
        bld.putCMD(static_cast<uint32_t>(InCommandWrap::ECommands::command_VTM));

        //measurementsToReturn contains how many. First one returns difference of current to timestamp
        HKTime::UpTime diff = Sleeper::getUpTime();
        TempMeasure::TempRecord tempRecord = TempMeasure::getTempMeasurementRecord(0);
        diff = diff - (HKTime::UpTime)tempRecord.timeStamp;

        bld.addMeasurement(HKTime::SmallUpTime(diff), tempRecord.tempFPCelcjus);

        measurementsToReturn--; //one is returned in bld

                                //now set up records handler
        if (measurementsToReturn > 0)
        {
            HKCommExtraRecordsHDL::setNumRecords(measurementsToReturn);
            HKCommExtraRecordsHDL::setDataReciever(&HKCommExtraHLRs::RTHdataReciever);
        }
    }
}

void HKComm::command_AVI( OutBuilder & bld)
{
    bld.putCMD(static_cast<uint32_t>(InCommandWrap::ECommands::command_AVI));
    static const char v[] ={ ' ','0','.','6','.','1' };

    bld.addData(v, NUM_ELS(v));

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
    0.5.1.4 Development (11,280) bytes
    +Added compatible commands as macro
    !not tested on system
    0.5.1.5 Development (11,280)
    + 
    0.5.2.0
    improved power save mode
    //see http://www.home-automation-community.com/arduino-low-power-how-to-run-atmega328p-for-a-year-on-coin-cell-battery/
    0.5.2.1
    LED off capability
    //see http://www.home-automation-community.com/arduino-low-power-how-to-run-atmega328p-for-a-year-on-coin-cell-battery/

    0.6.0 change to protocol!. Not all previous commands are supported
    0.6.1 change to protocol!. Restored previous commands

    0.?.1
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
 
}

//------------------------------------------------------------------

