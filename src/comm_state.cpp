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


#include "serial.h"
#include "comm.h"
#include "supp.h"
#include "comm_state.h"


HKCommState::HKCommState()
    : m_state       (ESerialState::serialState_Preable)
    , m_errorType   (ESerialErrorType::serialErr_None)
    , m_errSubtype  (0)
{}


void HKCommState::setState(ESerialState state)
{
    m_state          =   state;
    m_errorType      =   ESerialErrorType::serialErr_None;
    m_errSubtype     =   0;
}

bool HKCommState::isError() const
{
    return m_errorType != ESerialErrorType::serialErr_None;
}

HKCommState::ESerialState  HKCommState::getState() const
{
    return m_state;
}
HKCommState::ESerialErrorType  HKCommState::getErrorType() const
{
    return m_errorType;
}

uint8_t  HKCommState::getErrSUBType() const
{
    return m_errSubtype;
}

void  HKCommState::setErrorState(ESerialErrorType error)
{
    m_state          =   ESerialState::serialState_Error ;
    m_errorType      =   error;

    m_errSubtype     =   0;
}

void  HKCommState::setErrorState(OutBuilder::ELogicErr error)
{
    setErrorState(HKCommState::ESerialErrorType::serialErr_LogicCall);
    m_errSubtype     =   static_cast<uint8_t>(error);
}

void  HKCommState::setErrorState(ParseResult error)
{
    setErrorState(HKCommState::ESerialErrorType::serialErr_Parser);
    m_errSubtype     =   static_cast<uint8_t>(error);
}




//------------------------------------------------------------------
