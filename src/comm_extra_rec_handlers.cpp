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
#include "temp_measurement.h"
#include "comm_extra_rec_handlers.h"

//@brief a function passed to HKCommExtraRecordsHDL so it returns extra records
HKCommExtraHLRs::exitCode HKCommExtraHLRs::RTHdataReciever(HKTime::SmallUpTime & timeReturned, int16_t & value, uint16_t whichRecordBack)
{
    if (whichRecordBack == 0)
    {
        return HKCommExtraHLRs::exitCode::generalError;
    }
    timeReturned =    TempMeasure::getTempMeasurementRecord(whichRecordBack -1).timeStamp //newer
        - TempMeasure::getTempMeasurementRecord(whichRecordBack   ).timeStamp;  //older (current temp)
                                                                                //subtracting older from newer 
    value        =  TempMeasure::getTempMeasurementRecord(whichRecordBack).tempFPCelcjus;

    if (value == TempMeasure::tempMeasurement_invalid)
    {
        return HKCommExtraHLRs::exitCode::noValidData;
    }
    return HKCommExtraHLRs::exitCode::ok;
}
