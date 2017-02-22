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
#include "comm_common.h"


int16_t                             HKCommExtraRecordsHDL::recordsIt   = 0;
int16_t                             HKCommExtraRecordsHDL::totalRecords   = 0;
HKCommExtraRecordsHDL::DataReciever  HKCommExtraRecordsHDL::dataReciever      = 0;

//@brief returs formatted string in outData and increments the inOutOffset with amount of chars.
// in valid returs if record is valid or run ouf of scheduled elems
uint8_t HKCommExtraRecordsHDL::formatedMeasurement(uint8_t & valid,
                                             uint16_t & inOutOffset, 
                                             uint8_t (&outData)[HKCommDefs::commandMaxDataSize])
{
    valid = 0;
    HKTime::SmallUpTime timeReturned;
    int16_t  value;
    if (dataReciever == 0 || recordsIt >= totalRecords  )
    {
        return HKCommDefs::serialErr_None;
    }
    uint8_t err = dataReciever(timeReturned, value, recordsIt + 1);
    recordsIt++;
    if (err != HKCommDefs::serialErr_None)
    {
        return err;
    }
    err  = HKCommCommon::formatMeasurement(inOutOffset, outData, timeReturned, value);
    if (err == HKCommDefs::serialErr_None)
    {
        valid = 1;
    }
    return err;
}

void HKCommExtraRecordsHDL::setNumRecords(uint16_t records)
{
    totalRecords = records;
    recordsIt = 0;
}

