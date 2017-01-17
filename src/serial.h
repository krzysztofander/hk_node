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
#ifndef HK_SERIAL_H
#define HK_SERIAL_H

#include <Arduino.h>
#include "hk_node.h"

class HKComm
{
public:
    static const uint8_t commandEOLSignOnRecieve  = 0x0dU;
    static const uint8_t commandEOLOnResponceSequence[2]; //sequence send as an end of line on response


    enum ECommandsConsts
    {
        commandSize = 3,

        commandIdentifierPos = 0,
        command_subIdPos1 = 1,
        command_subIdPos2 = 2,


        command_DataSize = 8,
        commandEOLSizeOnRecieve = sizeof(commandEOLSignOnRecieve), //how many uint8_tacters to expect on end of line
        commandEOLSizeOnResponce = sizeof( commandEOLOnResponceSequence),
        commandMaxDataSize = command_DataSize + commandEOLSizeOnRecieve

    };

    enum ESerialState
    {
        serialState_ReadCommand,
        serialState_ReadData,
        serialState_Action,
        
        serialState_Respond,
        serialState_Error,
    };
    
    enum ESerialErrors
    {
        serialErr_None,

        serialErr_Assert = 1,           // some assertion triggered

        serialErr_IncorrectNumberFormat = 2,  //format of the number from serial is incorrect. 
        serialErr_NumberToShort =3 ,    //a number recieved from serial is to short. Recieved EOL to early

        serialErr_eolInCommand =4 ,
        serialErr_noEolFound = 5,
        serialErr_UnknownCommand =6,   //a command recieved is not recognized
        serialErr_WriteFail = 7,     //a number of bytes written is not same as expected

    };


    
    static uint8_t dataToUnsignedShort(uint8_t offset, const uint8_t (&inData)[commandMaxDataSize], uint16_t & retVal );
    static uint8_t command_D(uint8_t (&inOutCommand)[commandSize], uint8_t (&inOutData)[commandMaxDataSize], uint8_t & dataSize);
    static uint8_t command_C(uint8_t (&inOutCommand)[commandSize], uint8_t (&inOutData)[commandMaxDataSize], uint8_t & dataSize);
    static uint8_t respondSerial(void);
    static void echoLetter(uint8_t l);
    static uint8_t isActive(void);

    static uint8_t g_command[commandSize];
    static uint8_t g_data[commandMaxDataSize];
    static uint8_t g_dataIt;

    static uint8_t g_SerialState;
    static uint8_t g_serialError;


};
#endif
