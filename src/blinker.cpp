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
#include "hk_node.h"
#include "supp.h"
#include "blinker.h"
#include "comm_common.h"
#include "comm.h"
//------------------------------------------------------------------
uint8_t Blinker::g_blinkSetting = 1;
void Blinker::init()
{
    g_blinkSetting = 1;
}

void Blinker::blinkAction()
{
    if (Supp::isButtonPressed())
    {
        g_blinkSetting++;
        if (g_blinkSetting > 4)
        {
            g_blinkSetting  = 0;
            Supp::blinkLed(0x81); //just confirm here and forget till changed
        }
    }
 

    uint8_t pattern = 0;
    for (uint8_t i = 0; i < g_blinkSetting && i < 8; i++)
    {
        pattern <<= 1;
        pattern  |= 1;
    }

    Supp::blinkLed(pattern);
    

    if (!Supp::isButtonPressed())
    {
        //various ag_blinkSettingctions for particular settings, that has been selected blink ago
        switch (g_blinkSetting)
        {
            case 2: //keep sending messages to serial
            {
                static int8_t couter = 0;

                uint16_t dataSize = 0;
                HKComm::g_data[dataSize++] = 'A';
                HKComm::g_data[dataSize++] = 'H';
                HKComm::g_data[dataSize++] = 'L';
                HKCommCommon::uint8ToData(dataSize, HKComm::g_data, couter++);

                Serial.write(HKComm::g_data, dataSize);
                Serial.write(HKCommDefs::commandEOLOnResponceSequence, NUM_ELS(HKCommDefs::commandEOLOnResponceSequence));
            }

            break;
            case 3: 
                g_blinkSetting = 1;
                break; 
                //do not do anuthing for now
            case 4:  
                g_blinkSetting = 1;
                break; 
            //if it was sth else leave it.
        }
    }
}
