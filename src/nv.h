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
#ifndef NV_H
#define NV_H
#include <Arduino.h>
#include "hk_node.h"
//------------------------------------------------------------------

class NV
{
public:
    ENUM( NVData )
    {
        nvTestProgrammed = 0,
        nvBTName         = 1,
        nvBandgapVoltage = 2

    };
    ENUM( NVDataSize )
    {
        nvTestProgrammed    = sizeof(uint32_t),
        nvBTName            = 12 + 1 /*for \x0*/,
        nvBandgapVoltage    = sizeof(uint16_t),
    };
private:

    SHORTENUM(NVDataAddr)
    {
       base = 0,
       nvTestProgrammed = static_cast<uint16_t>(base),

       nvBTName  =   static_cast<uint16_t>(nvTestProgrammed)
                   + static_cast<uint16_t>(NVDataSize::nvTestProgrammed),

       nvBandgapVoltage =    static_cast<uint16_t>(nvBTName)
                           + static_cast<uint16_t>(NVDataSize::nvBTName),

       //next =    
       //          
       
    };

    struct NVDescr
    {
        NVDescr()
            : size(0),
            address(0),
            stopAt0(false)
        {}
        uint8_t size ;
        uint16_t address;
        bool stopAt0 ;
    };

    static void getDesrc(NVData what, NVDescr & dsr);
    static const uint32_t g_testProgrammed;

public:

    static void save(NVData what, const void * dataToSave);
    static void read(NVData what, void * dataToLoad);
    static void init();
    static void forceFactoryDefaults();
};


//---------------------------------------------------------------
#endif