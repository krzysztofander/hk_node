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
#ifndef HK_COMM_H
#define HK_COMM_H

#include "hk_node.h"
#include "comm_defs.h"
#include "comm_common.h"
#include "MiniInParser.h"
#include "out_builder.h"

class InCommandWrap : public Command
{
public:
    uint16_t getUint16(uint8_t & err)
    {
        uint16_t ret = 0;
        if (outParamType != OutParamType_INT_DIGIT)
        {
            err = (uint8_t)HKCommDefs::serialErr_DataType_NumberExpected;
        }
        else
        {
            if (numericValue < 0)
            {
                err = (uint8_t)HKCommDefs::serialErr_DataType_UnsignedExpected;
            }
            else
            {
                err = (uint8_t)0;
                ret = static_cast<uint16_t> (numericValue);
            }
        }
        return ret;
    }

};


class HKComm
{
public:
    
    static uint8_t formatResponceUnkL1(uint8_t (&inOutCommand)[HKCommDefs::commandSize], uint16_t & dataSize);
    static uint8_t formatResponceUnkL2(uint8_t (&inOutCommand)[HKCommDefs::commandSize], uint16_t & dataSize);
    static uint8_t formatResponceOK(uint8_t (&inOutCommand)[HKCommDefs::commandSize], uint8_t (&inOutData)[HKCommDefs::commandMaxDataSize], uint16_t & dataSize);


    static uint8_t command_D(uint8_t (&inOutCommand)[HKCommDefs::commandSize], uint8_t (&inOutData)[HKCommDefs::commandMaxDataSize], uint16_t & dataSize);
    static uint8_t command_C(uint8_t (&inOutCommand)[HKCommDefs::commandSize], uint8_t (&inOutData)[HKCommDefs::commandMaxDataSize], uint16_t & dataSize);
    static uint8_t command_R(uint8_t (&inOutCommand)[HKCommDefs::commandSize], uint8_t (&inOutData)[HKCommDefs::commandMaxDataSize], uint16_t & dataSize);
    
    static uint8_t command_RVI(uint8_t (&inOutCommand)[HKCommDefs::commandSize], uint8_t (&inOutData)[HKCommDefs::commandMaxDataSize], uint16_t & dataSize);


    static uint8_t command_RTM(uint8_t (&inOutCommand)[HKCommDefs::commandSize], uint8_t (&inOutData)[HKCommDefs::commandMaxDataSize], uint16_t & dataSize);
    static uint8_t command_RTH(uint8_t (&inOutCommand)[HKCommDefs::commandSize], uint8_t (&inOutData)[HKCommDefs::commandMaxDataSize], uint16_t & dataSize);


    //sets serial state to action after specific command
    //can be used to send responsce w/o  request for debug purposes
    static void jumpToAction(const uint8_t * command, const  uint8_t * data, const uint16_t dataSize);
    
    //sets serial state to action after specific command
    //can be used to immediately send something responsce w/o  request for debug purposes
    static void jumpToResp(const uint8_t * command, const  uint8_t * data, const uint16_t dataSize);

    static bool respondSerial(void);
    static void echoLetter(uint8_t l);
    static uint8_t isActive(void);

    static uint8_t g_command[HKCommDefs::commandSize];
    static uint8_t g_data[HKCommDefs::commandMaxDataSize];
    static uint16_t g_dataIt;

    static uint8_t g_SerialState;
    static uint16_t g_serialError;

    static InCommandWrap g_RecievedCmd;
    static OutBuilder g_OutBuilder;
    static void command_DED(OutBuilder & bld);
    static void command_RTH(OutBuilder & bld, uint16_t elements);


};
#endif
