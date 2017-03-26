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
    BIGENUM(ECommands)
    {
        command_CTR = 0x435452,  //temperature resolution
        command_CTP = 0x435450,  //temperature perion
        command_CBP = 0x435042,  //blinker period
                      
        command_CBS = 0x435053,  //Blinker settings (pattern)
                      
        command_CPP = 0x435050,  //power(batery level) period
        command_CST = 0x435354,  //system time
        command_CNN = 0x434e4e,  //node name (string)
        command_CRS = 0x435253,  //reset
        command_CSM = 0x43534d,  //power saving mode
        command_CSA = 0x435641,  //power down inactivity
        command_AVI = 0x415649,  //aux version information
        command_RVI = 0x525649,  //read version information (deprecated)
        command_RTH = 0x525448,  //read temperature history
        command_RTM = 0x52544d,  //undocumented
        command_DER = 0x444552,
        command_DLY = 0x444c59,  //LEDS yes  
        command_DLN = 0x444c4e,  //LEDS no
#if HAVE_HUMAN_READABLE
        command_AHR = 0x414852,  //switch on human readable mode
#endif
        
    };
    /*
    uint32_t        cmd;
    OutParamType    outParamType;
    union
    {
    uint64_t        numericValue;
    struct
    {
    char*           stringValue;
    uint8_t         stringValueMaxLen;
    };
    };
    */

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
    ECommands getCommand () const
    {
        return static_cast<ECommands>(cmd >> 8);
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

};

#endif
