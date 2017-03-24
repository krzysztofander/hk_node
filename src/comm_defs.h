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
#ifndef HK_COMM_DEFS_H
#define HK_COMM_DEFS_H

#include "hk_node.h"

class HKCommDefs
{
public:
    static const uint8_t preamble  = '!';
    static const uint8_t commandEOLSignOnRecieve  = 0x0dU;
    static const uint8_t commandEOLOnResponceSequence[2]; //sequence send as an end of line on response


    enum ECommandsConsts
    {
        commandSize = 3,

        commandIdentifierPos = 0,
        command_subIdPos1 = 1,
        command_subIdPos2 = 2,


        commandEOLSizeOnRecieve = sizeof(commandEOLSignOnRecieve), //how many uint8_tacters to expect on end of line
        command_DataSize = 64 - commandEOLSizeOnRecieve - sizeof(uint16_t),

        commandEOLSizeOnResponce = sizeof( commandEOLOnResponceSequence),
        commandMaxDataSize = command_DataSize + commandEOLSizeOnRecieve

    };

    enum ESerialState
    {
        serialState_Preable = 0,
        serialState_ParseCommand = 1,
        serialState_Action = 2,
        serialState_Respond = 3,
        serialState_Error = 4,
    };
    
    enum ESerialErrors
    {
        serialErr_None,

        serialErr_Assert    = 0x100,           // some assertion triggered
        serialErr_Parser    = 0x200,           // parser could not reconize command
        serialErr_Logic     = 0x300,           // command could not be executed
        serialErr_WriteFail = 0x400,     //a number of bytes written is not same as expected



        serialErr_Assert_NotImpl = 0x11,           // not implemented yet
        serialErr_UnknownCommand = 0x60,   //a command recieved is not recognized

        
        serialErr_Number_IncorrectFormat = 0x20,  //format of the number from serial is incorrect. 
        serialErr_Number_Uint16ToShort   =0x21 ,    //a number recieved from serial is to short. Recieved EOL to early
        serialErr_Number_Uint32ToShort   =0x22 ,    //a number recieved from serial is to short. Recieved EOL to early
        serialErr_Number_NoCorrectLength =0x23 ,


        serialErr_eolInCommand =0x40 ,
        serialErr_noEolFound = 0x50,
        serialErr_UnknownCommand =0x60,   //a command recieved is not recognized

    };


    enum ECommands
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
        command_RTH,
        command_DED = 0x44454400,
        command_DL0,
        command_DL1,
    }

































































    }

};
#endif
