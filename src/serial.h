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

#include "hk_node.h"

class HKSerial
{
    ENUM(PreableState)
    {
        none,
            nonRecognizedChar,
            exclamation,
            finished,
            okLost_O,
            okLost_S,
            okLost_T,

    };

    static PreableState  g_preableState;
    static int8_t preableFinishTime;
    static void advanceAndHandleSMState();

    static void traverseSM(char charRead);
    static void resetSM();
    static bool preambleRecieved();
    static void sendReadBT(const char * command, int8_t size);
public:
    static void nextLoop(uint8_t tickCnt);
    static void init();
    static void BTinit();
    static uint8_t read();
    static uint8_t available();
    static bool checkActive();      //! Indicates that transmission, e.g. preable started
    static void commandProcessed(); //! Information from upper layer that it finished
    static uint8_t peek();
    static uint8_t write(const uint8_t * buff, size_t size);

    static void flush();
    static void end();
    static void begin(int speed);


};
#endif
