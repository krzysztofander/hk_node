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
#include "serial.h"
#include "comm.h"
#include "supp.h"
#include "comm_common.h"
#include "comm_extra_records.h"
#include "comm_extra_rec_handlers.h"
#include "MiniInParser.h"

char *OutBuilder::itoa(int i)
{
    enum
    {
        INT_DIGITS = 19,
    };
    /* Room for INT_DIGITS digits, - and '\0' */
    static char buf[INT_DIGITS + 2];
    char *p = buf + INT_DIGITS + 1;	/* points to terminating '\0' */
    if (i >= 0) {
        do {
            *--p = '0' + (i % 10);
            i /= 10;
        } while (i != 0);
        return p;
    }
    else {			/* i < 0 */
        do {
            *--p = '0' - (i % 10);
            i /= 10;
        } while (i != 0);
        *--p = '-';
    }
    return p;
}

void OutBuilder::putCMD(const char * cmd)
{
    dataSize  = 0; //reset
    inOutCommand[0] = cmd[0];
    inOutCommand[1] = cmd[1];
    inOutCommand[2] = cmd[2];
}

void OutBuilder::putData(const char * data, const uint16_t size)
{
    dataSize = size;
    for (int i = 0; i < dataSize; i++)
    {
        inOutData[i] = data[i];
    }
}

//@brief formats the measurement time/val pair directly into buffer
//@param [inout] inOutOffset:  base and new offset in buffer
//@param [inout] inoutData:  buffer where to input the function
//@param [in ] inoutDataSize:  size of buffer

void OutBuilder::putMeasurement( HKTime::SmallUpTime timeStamp, int16_t val)
{
    static const char chStart = '(';
    static const char chSeparator = ',';
    static const char chEnd   = ')';

    dataSize = 0;

    inOutData[dataSize++]=chStart;
    HKCommCommon::shortToData(dataSize, inOutData, uint16_t(timeStamp >> 16)); //MSB first
    HKCommCommon::shortToData(dataSize, inOutData, uint16_t(timeStamp));       //LSB
    inOutData[dataSize++]=chSeparator;
    HKCommCommon::shortToData(dataSize, inOutData, val);
    inOutData[dataSize++]=chEnd;
}

uint8_t OutBuilder::getError()
{
    return err;
}

void OutBuilder::putInt(uint32_t newInt)
{
    dataSize = 0;
    HKCommCommon::uint32ToData(dataSize, inOutData, newInt);
}
