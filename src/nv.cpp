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
#include <Arduino.h>
#include "nv.h"
#include <EEPROM.h>
//------------------------------------------------------------------
void NV::getDesrc(NV::NVData what, NV::NVDescr & dsr)
{
    dsr.size = 0;
    dsr.stopAt0 = false;


    switch (what)
    {
        case NV::NVData::nvTestProgrammed:

            dsr.size        = static_cast<uint8_t> (NV::NVDataSize::nvTestProgrammed);
            dsr.address     = static_cast<uint16_t>(NV::NVDataAddr::nvTestProgrammed);
            break;

        case NV::NVData::nvBTName:
            dsr.size        = static_cast<uint8_t> (NV::NVDataSize::nvBTName);
            dsr.address     = static_cast<uint16_t>(NV::NVDataAddr::nvBTName);
            dsr.stopAt0     = true;
            break;

        default:
            //this is an error, do nothing
            break;
    }


}


void NV::save(NV::NVData what, const void * dataToSave)
{
    NV::NVDescr dsr;
    getDesrc(what, dsr);
    
    for (uint8_t i = 0; i < dsr.size; i++)
    {
        uint8_t c = ((uint8_t*)dataToSave)[i];
        EEPROM.write(dsr.address + i, c);
        if (dsr.stopAt0 && (c == '\x0'))
        {
            break;
        }
    }
    
  
}
void NV::read(NV::NVData what, void * dataToLoad)
{
    NV::NVDescr dsr;
    getDesrc(what, dsr);

   
    for (uint8_t i = 0; i < dsr.size; i++)
    {
        uint8_t c = EEPROM.read(dsr.address + i);
        ((uint8_t*)dataToLoad)[i] = c;
        if (dsr.stopAt0 && (c == '\x0'))
        {
            break;
        }
    }

}

const uint32_t NV::g_testProgrammed = 0x12345678;

void NV::forceFactoryDefaults()
{
    //need to put default values to eeprom
    NV::save(NV::NVData::nvBTName, "HCNode" );


    //finally the marker
    NV::save(NV::NVData::nvTestProgrammed, &g_testProgrammed );

}


void NV::init()
{
    //BT name:
    uint32_t ui32Data;
    NV::read(NV::NVData::nvTestProgrammed, &ui32Data);
    if (g_testProgrammed != ui32Data)
    {
        forceFactoryDefaults();
    }

}



//---------------------------------------------------------------
