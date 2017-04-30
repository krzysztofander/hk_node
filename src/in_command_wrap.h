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

#include "hk_node_utils.h"
#include "MiniInParser.h"
#include "out_builder.h"
#include "hk_node_config.h"
#include "commands.h"

class InCommandWrap : public Command
{
public:
     static const uint8_t maxReadStringSize = 16; 

    int64_t getInt(OutBuilder::ELogicErr & err,
                   const bool isSigned,
                   const uint8_t byteRange,
                   const bool positiveOnly)   const;
   

    const char * getString(OutBuilder::ELogicErr & err) const;

    const int8_t getMaxString() const
    {
        return (int8_t)NUM_ELS(g_strBuff);
    }


    ECommands getCommand () const
    {
        return static_cast<ECommands>(cmd >> 8);
    }
    bool hasData() const
    {
        return (OutParamType_NONE != outParamType);
    }

    void setCommand (ECommands newCmd)
    {
        cmd =  static_cast<uint32_t>(newCmd) << 8;
    }
    void setIntData (int16_t cmd)
    {
        outParamType = OutParamType_INT_DIGIT;
        numericValue = (uint64_t)((int64_t) cmd);
    }

    void reset()
    {
        stringValue = g_strBuff;
        stringValueMaxLen =  getMaxString();

    }
    char g_strBuff[maxReadStringSize];  //null terminated
    
};

#endif
