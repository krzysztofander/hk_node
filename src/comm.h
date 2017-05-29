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

#include "out_builder.h"
#include "comm_state.h"
#include "in_command_wrap.h"
#include "hk_node_config.h"


class HKComm
{
public:
    static bool respondSerial(void);
    static void actionState(void);
    //static void echoLetter(uint8_t l);
    static bool isActive(void);

    static void jumpToResp(void);     //@!Immediately set serial state to response
    static void jumpToAction(void);   //@!Immediately set serial state to action

    static void command_DER(OutBuilder & bld);
    static void command_RTH(const InCommandWrap & inCmd, OutBuilder & bld);
    static void command_RTM(OutBuilder & bld); 
    static void command_AVI(OutBuilder & bld);
#if HAVE_HUMAN_READABLE
    static void command_AHR(OutBuilder & bld);
#endif
    static void commandCTP(const InCommandWrap & inCmd, OutBuilder & bld);
    static void commandCBP(const InCommandWrap & inCmd, OutBuilder & bld);
    static void commandCBS(const InCommandWrap & inCmd, OutBuilder & bld);
    static void commandCSM(const InCommandWrap & inCmd, OutBuilder & bld);
    static void commandCST(const InCommandWrap & inCmd, OutBuilder & bld);
    static void commandCSC(const InCommandWrap & inCmd, OutBuilder & bld);

    static void commandCSA(const InCommandWrap & inCmd, OutBuilder & bld);
    static void commandCNN(const InCommandWrap & inCmd, OutBuilder & bld);
    static void commandDLS(const InCommandWrap & inCmd, OutBuilder & bld);

    static void commandRPM(const InCommandWrap & inCmd, OutBuilder & bld);
    static void commandCRV(const InCommandWrap & inCmd, OutBuilder & bld);
    

    static OutBuilder & accessOutBuilder()
    {
        return g_OutBuilder;
    }

    static InCommandWrap & accessInCommandWrap()
    {
        return g_RecievedCmd;
    }

    static HKCommState      g_commState;           //!@ state of the machine
    static InCommandWrap    g_RecievedCmd;         //!@ Input parser
    static OutBuilder       g_OutBuilder;          //!@ Output builder


};
#endif
