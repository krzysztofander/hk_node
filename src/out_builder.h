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
#ifndef HK_OUT_BUILDER_H
#define HK_OUT_BUILDER_H

#include "hk_node.h"
#include "comm_defs.h"
#include "MiniInParser.h"
class OutBuilder
{
public:
    ENUM (Consts)
    {
        BufferSize = 64,

    };

    ENUM (ELogicErr)
    {
        None,
        UnsignedExpected,
        NumberExpected,
        BufferOverrun,
    };

    static char *itoa(int64_t i    , uint16_t & strSizeOut);

    void putErr(ELogicErr err);
    ELogicErr getError() const;


    bool isErr();

    void putCMD(const char * cmd);
    void addData(const char * data, const uint16_t size);
    void addInt(int64_t newInt);
    void addMeasurement(HKTime::SmallUpTime timeStamp, int16_t val);

    void reset();

    const int getStrLenght() const;
    const uint8_t * getStrToWrite() const;
#if HAVE_HUMAN_READABLE
    void setHumanReadable()
    {
        g_HumanReadableMode = true;
    }
    static bool g_HumanReadableMode; 
#endif


private:  
    uint8_t m_buffer[static_cast<int>(Consts::BufferSize)];
    uint16_t m_dataSize;
    
    ELogicErr m_err;

   
}; 
#endif
