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

#include "in_command_wrap.h"

int64_t InCommandWrap::getInt(OutBuilder::ELogicErr & err,
               const bool isSigned,
               const uint8_t byteRange, 
               const bool positiveOnly)   const
{
    int64_t ret =0;
    if (outParamType != OutParamType_INT_DIGIT)
    {
        err = OutBuilder::ELogicErr ::NumberExpected;
    }
    else
    { 
        ret = static_cast<int64_t>(numericValue);

        if ( (!isSigned || positiveOnly )&& ret < 0)
        {
            err = OutBuilder::ELogicErr ::UnsignedExpected;
        }
        else 
#define BITMASK (~0ULL << ((byteRange * 8) - (isSigned ? 1 : 0)) )
            if 
                (
                (  (ret >= 0 )
                && (ret & BITMASK) != 0 )
                ||
                (  (ret < 0 )
                && (ret & BITMASK) != BITMASK )
                )
#undef BITMASK
            {
                err = OutBuilder::ELogicErr::SettingToBig;
            }
            else
            {
                err = OutBuilder::ELogicErr ::None;
            }
    }
    return ret;
}

const char * InCommandWrap::getString(OutBuilder::ELogicErr & err) const
{
    if (outParamType != OutParamType_STRING)
    {
        err = OutBuilder::ELogicErr::StringExpected;
    }
    else
    {
        err = OutBuilder::ELogicErr ::None;
    }
    return stringValue;
}