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
#ifndef HK_IN_COMMAND_WRAP_H
#define HK_IN_COMMAND_WRAP_H

#include "hk_node.h"
#include "MiniInParser.h"
#include "out_builder.h"

class InCommandWrap : public Command
{
public:
    BIGENUM( ECommands)
    {
        command_CTR,
        command_CTP,
        command_CBP,
        command_CPP,
        command_CST,
        command_CNN,
        command_CRS,
        command_CSM,
        command_CSA,
        command_AVI,
        command_RTH,
        command_DED = 0x444544,
        command_DL0,
        command_DL1,
    };


    uint16_t getUint16(OutBuilder::ELogicErr & err)
    {
        uint16_t ret = 0;
        if (outParamType != OutParamType_INT_DIGIT)
        {
            err = OutBuilder::ELogicErr ::NumberExpected;
        }
        else
        {
            if (numericValue < 0)
            {
                err = OutBuilder::ELogicErr ::UnsignedExpected;
            }
            else
            {
                err = OutBuilder::ELogicErr ::None;
                ret = static_cast<uint16_t> (numericValue);
            }
        }
        return ret;
    }
    ECommands getCommand ()
    {
        return static_cast<ECommands>(cmd >> 16);
    }
};

#endif
