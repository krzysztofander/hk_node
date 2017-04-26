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
        command_CTR = 0x435452,  //!temperature resolution          NOT implemented
        command_CTP = 0x435450,  //!temperature period                  OK
        command_CBP = 0x434250,  //!blinker period                      OK
                      
        command_CBS = 0x434253,  //!Blinker settings (pattern)          OK
                      
        command_CPP = 0x435050,  //power(batery level) period       NOT implemented
        command_CST = 0x435354,  //!system time                         OK
        command_CSC = 0x435343,  //!system capabilities (read only)     OK
        command_CNN = 0x434e4e,  //node name (string)                   OK
        command_CRS = 0x435253,  //reset                            NOT implemented
        command_CSM = 0x43534d,  //!power saving mode                   OK
        command_CSA = 0x435341,  //!power down inactivity               OK
        command_AVI = 0x415649,  //!aux version information             OK
        command_RVI = 0x525649,  //!read version information (deprecated) 
        command_RTH = 0x525448,  //!read temperature history            OK
        command_RTM = 0x52544d,  //!undocumented
        command_VTM = 0x56544d,  //!return temperature values           OK
        command_DER = 0x444552,  //!return Debug Echo Responce          OK
        command_DLS = 0x444c53,  //LEDS status                          OK

        command_RPM = 0x52504d,  //read power measurement (battery level) ~ongoing
        command_CRV = 0x435256,  //configure reference voltage            ~ongoing

        command_VPM = 0x56504d,  //return read power measurement 
                                 //(battery level) in mV                  ~ongoing


#if HAVE_HUMAN_READABLE
        command_AHR = 0x414852,  //switch on human readable mode        OK
#endif
        command_ERR = 0x455252,  //Return an error
    };

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
