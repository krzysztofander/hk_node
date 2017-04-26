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
#include "comm.h"
#include "in_command_wrap.h"
#include "sleeper.h"
//------------------------------------------------------------------
uint8_t Blinker::g_blinkSetting = 1;
//------------------------------------------------------------------
void Blinker::init()
{
    g_blinkSetting = 1;
}
//------------------------------------------------------------------
void Blinker::setBlinkPattern(uint8_t pattern)
{
    g_blinkSetting = pattern;
}
//------------------------------------------------------------------
uint8_t Blinker::getBlinkPattern()
{
    return g_blinkSetting;
}
//------------------------------------------------------------------

void Blinker::blinkAction()
{
    if (Supp::isButtonPressed())
    {
        //This is to manually change blink setting to something recognizable

        g_blinkSetting <<= 1;
        g_blinkSetting  |=  1;
        
        if ((uint8_t)(g_blinkSetting & (uint8_t)0x1F) == (uint8_t)0x1F) //4 bits set
        {
            g_blinkSetting  = 0;
            Supp::blinkLed(0x81); //just confirm here and forget till changed
        }
    }
 

    Supp::blinkLed(g_blinkSetting);
    

    if (!Supp::isButtonPressed())
    {
        //various ag_blinkSettingctions for particular settings, that has been selected blink ago
        switch (g_blinkSetting)
        {
            case 0x0: 
                break;
            case 0x1: 
                break;
            case 0x3: //keep sending messages to serial
            {
                static uint8_t counter = 0;
                if (0 ||
                    (uint8_t)(counter & (uint8_t)0x03) == (uint8_t)0)
                {
                    HKComm::accessInCommandWrap().setCommand(InCommandWrap::ECommands::command_RTM);
                    HKComm::accessInCommandWrap().setIntData(0);
                    HKComm::jumpToAction();
                }
                else
                {
                    HKComm::accessInCommandWrap().setCommand(InCommandWrap::ECommands::command_RPM);
                    HKComm::accessInCommandWrap().setIntData(0);
                    HKComm::jumpToAction();


                    //HKComm::accessInCommandWrap().setCommand(InCommandWrap::ECommands::command_DER);
                    //HKComm::accessInCommandWrap().setIntData(0);
                    //HKComm::jumpToAction();
                }
                counter++;
            }

            break;
            case 7: 
                //some action can be added here e.g. reset BT & stuff to default
                if (Sleeper::getPowerSaveMode() == Sleeper::PowerSaveMode::high)
                {
                    Sleeper::setPowerSaveMode(Sleeper::PowerSaveMode::medium);
                }
                else
                {
                    Sleeper::setPowerSaveMode(Sleeper::PowerSaveMode::high);
                }
                g_blinkSetting = 1;   
                break; 
            case 0xf: 
                //some action can be added here e.g. reset BT & stuff to default
                g_blinkSetting = 1;   
                Supp::extLEDMasterCtrl( ! Supp::getExtLEDMasterCtrl() );
                break; 
                //do not do anything for now
        }
    }
}
//------------------------------------------------------------------
