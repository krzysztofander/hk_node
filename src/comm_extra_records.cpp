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

#include "comm_extra_records.h"


int16_t                             HKCommExtraRecordsHDL::recordsIt   = 0;
int16_t                             HKCommExtraRecordsHDL::totalRecords   = 0;
HKCommExtraRecordsHDL::DataReciever  HKCommExtraRecordsHDL::dataReciever      = 0;


HKCommExtraRecordsHDL::exitCodes HKCommExtraRecordsHDL::formatedMeasurement(bool & valid, OutBuilder & bld)

{
    valid = false;
    HKTime::SmallUpTime timeReturned;
    int16_t  value;
    if (dataReciever == 0 || recordsIt >= totalRecords  )
    {
        //run out of data
        return HKCommExtraRecordsHDL::exitCodes::ok;
    }
    auto exitCode = dataReciever(timeReturned, value, recordsIt + 1);
    recordsIt++;
    if (exitCode == HKCommExtraHLRs::exitCode::noValidData)
    {
        return HKCommExtraRecordsHDL::exitCodes::noMoreData;
    }
    else if (exitCode == HKCommExtraHLRs::exitCode::ok)
    {
        bld.addMeasurement(timeReturned, value);
        valid = true;
        return HKCommExtraRecordsHDL::exitCodes::ok;
    }
    else //if (exitCode == HKCommExtraHLRs::exitCode::generalError)
    {
        return HKCommExtraRecordsHDL::exitCodes::generalError;
    }
}

void HKCommExtraRecordsHDL::setNumRecords(uint16_t records)
{
    totalRecords = records;
    recordsIt = 0;
}

