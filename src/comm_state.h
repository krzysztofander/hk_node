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
#ifndef HK_COMM_STATE_H
#define HK_COMM_STATE_H

#include "hk_node.h"

#include "MiniInParser.h"
#include "out_builder.h"


class HKCommState
{
public:
    ENUM (ESerialState)
    {
        serialState_Preable = 0,
        serialState_ParseCommand = 1,
        serialState_Action = 2,
        serialState_Respond = 3,
        serialState_Error = 4,
    };

    ENUM (ESerialErrorType)
    {
        serialErr_None,
        serialErr_Assert    = 0x1,             // some assertion triggered
        serialErr_Parser    = 0x2,           // parser could not reconize command
        serialErr_LogicCall = 0x3,           // logic cannot be executed with parser data
        serialErr_LogicNoSuchCommand = 0x4,           // logic cannot be executed with parser data

        serialErr_Logic     = 0x4,           // command could not be executed
        serialErr_WriteFail = 0x5,           //a number of bytes written is not same as expected

    };

    HKCommState();
    void setState(ESerialState state);
    bool isError() const;
   
    ESerialState getState() const;
    
    ESerialErrorType getErrorType() const;
    
    uint8_t getErrSUBType() const;
   
    void setErrorState(OutBuilder::ELogicErr error);
    
    void setErrorState(ParseResult error);
    
    void setErrorState(ESerialErrorType error);
    

private:
    ESerialState             m_state;
    ESerialErrorType         m_errorType;
    uint8_t                  m_errSubtype;
};

#endif
