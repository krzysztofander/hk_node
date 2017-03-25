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

char *OutBuilder::itoa(int64_t i, uint16_t & strSizeOut)
{
    strSizeOut = 0;
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
            strSizeOut++;
        } while (i != 0);
        return p;
    }
    else {			/* i < 0 */
        do {
            *--p = '0' - (i % 10);
            i /= 10;
            strSizeOut++;
        } while (i != 0);
        *--p = '-';
        strSizeOut++;
    }
    return p;
}

bool OutBuilder::isErr()
{
    return m_err != ELogicErr::None;
}

void OutBuilder::reset()
{
    m_err = ELogicErr::None;
    m_dataSize = 0;
}

const int OutBuilder::getStrLenght() const
{
    return m_dataSize;
}
const uint8_t * OutBuilder::getStrToWrite() const
{
    return &m_buffer[0];
}


void OutBuilder::putCMD(const char * cmd)
{
    m_dataSize  = 3; //reset
    m_buffer[0] = cmd[0];
    m_buffer[1] = cmd[1];
    m_buffer[2] = cmd[2];
}

void OutBuilder::putErr(ELogicErr err)
{
    m_dataSize = 0;
    m_err = err;
}

void OutBuilder::addData(const char * data, const uint16_t size)
{
    if (m_dataSize + size > static_cast<int>(Consts::BufferSize))
    {
        putErr(ELogicErr::BufferOverrun);
    }
    else
    {
        for (uint16_t i = 0; i < size; i++)
        {
            m_buffer[m_dataSize + i] = data[i];
        }
        m_dataSize+= size;
    }
}

void OutBuilder::addInt(int64_t newInt)
{
    uint16_t strSizeOut;
    char * strInt = itoa( newInt,  strSizeOut);
    addData(strInt, strSizeOut);
}


void OutBuilder::addMeasurement( HKTime::SmallUpTime timeStamp, int16_t val)
{
    static const char chStart = '(';
    static const char chSeparator = ',';
    static const char chEnd   = ')';
    static const char chComma   = '.';
    
    static const uint8_t fractBits = 4; 
        //if we ever need to use different fracts, then we need to change that
        

    uint16_t strSizeOut;
    char * strInt;

    //parenthesis
    addData(&chStart, 1);

    //delta
    strInt = itoa( timeStamp,  strSizeOut);
    addData(strInt, strSizeOut);
    
    //colon
    addData(&chSeparator, 1);

    //int part
    strInt = itoa( val >>fractBits,  strSizeOut);
    addData(strInt, strSizeOut);

    //comma
    addData(&chComma, 1);
    
    //fract part
    if (val > 0)
    {
        val &= ~(~0u << fractBits);
    }
    else
    {
        val |= ~0u << fractBits;
        val = -val;
    }
    val *= 100;
    val /= 16;

    strInt = itoa( val ,  strSizeOut);
    addData(strInt, strSizeOut);
    //closing parethesis
    addData(&chEnd, 1); 

#if 1    
    addData((const char *)HKCommDefs::commandEOLOnResponceSequence, 
            NUM_ELS(HKCommDefs::commandEOLOnResponceSequence)
    );
    //
#endif

 }

OutBuilder::ELogicErr OutBuilder::getError() const
{
    return m_err;
}


