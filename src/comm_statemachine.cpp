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


#include "hk_node.h"
#include "serial.h"
#include "comm.h"
#include "supp.h"
#include "comm_common.h"
#include "comm_extra_records.h"
#include "comm_extra_rec_handlers.h"
#include "MiniInParser.h"


HKCommState      HKComm::g_commState;    
InCommandWrap    HKComm::g_RecievedCmd;  
OutBuilder       HKComm::g_OutBuilder;   

//------------------------------------------------------------------

uint8_t HKComm::isActive(void)
{
    if (g_commState.getState () != HKCommState::ESerialState::serialState_Preable
        || HKSerial::available() > 0)
        return 1;
    else
        return 0;
}


void HKComm::jumpToAction()
{
    g_commState.setState(HKCommState::ESerialState::serialState_Action);
}

void HKComm::jumpToResp()
{
    g_commState.setState(HKCommState::ESerialState::serialState_Respond);
}




// @brief Main function responding to serial data
// @returns True if switched state and shall be called immediately.

bool  HKComm::respondSerial(void)
{

    //alert(g_SerialState +1, false);
    switch (g_commState.getState())
    {
        case HKCommState::ESerialState::serialState_Preable:
            //we wait here for serial to stabilize, e.g. start communication
            //or send preamble
            
            //temporarily: when there is something available
            //in serial go to ParseCommand
            if (HKSerial::available() > 0)
            {
                g_commState.setState(HKCommState::ESerialState::serialState_ParseCommand);
                return true;
            }
            break;

        case HKCommState::ESerialState::serialState_ParseCommand:
        {
            if (HKSerial::available() > 0)
            {
                char currentChar = HKSerial::read();
                ParseResult parseResult  = miniInParse(currentChar, &g_RecievedCmd);
                if (parseResult != ParseResult_WILL_CONTINUE)
                {
                    if (parseResult == ParseResult_SUCCESS)
                    {
                        g_commState.setState(HKCommState::ESerialState::serialState_Action);
                    }
                    else
                    {
                        g_commState.setErrorState( parseResult  );

                     }
                    return true;
                }
            }
            break;
        }
        case HKCommState::ESerialState::serialState_Action:
        {
            g_OutBuilder.reset();
            switch (g_RecievedCmd.getCommand())
            {
                case InCommandWrap::ECommands::command_DED:
                {
                    command_DED(g_OutBuilder);
                }
                break;
                case InCommandWrap::ECommands::command_RTH:
                {
                    command_RTH(g_RecievedCmd, g_OutBuilder);
                }
                break;
                default:
                {
                    g_commState.setErrorState(
                        HKCommState::ESerialErrorType::serialErr_LogicNoSuchCommand);
                }

            }
            if (g_OutBuilder.isErr())
            {
                g_commState.setErrorState( g_OutBuilder.getError() );
            }
            if (! g_commState.isError())
            {
                g_commState.setState(HKCommState::ESerialState::serialState_Respond);
            }
            return true;
        }
        case HKCommState::ESerialState::serialState_Respond:
        {

            //write command then variable number of data then end of line sequence.
            uint16_t toWrite =  g_OutBuilder.getStrLenght();
            uint16_t written = HKSerial::write(
                g_OutBuilder.getStrToWrite(), g_OutBuilder.getStrLenght());
               
             //write extra records if any
            bool valid = 0;
            do
            {
                uint8_t ret = HKCommExtraRecordsHDL::formatedMeasurement(valid,g_OutBuilder);
                if (ret != 0)
                {
                    g_commState.setErrorState(
                        HKCommState::ESerialErrorType::serialErr_WriteFail );

                    return 1;
                }
                if (!valid)
                {
                    toWrite += g_OutBuilder.getStrLenght();
                    written += HKSerial::write(g_OutBuilder.getStrToWrite(), g_OutBuilder.getStrLenght());
                }
            } while (!valid);


            written += HKSerial::write(HKCommDefs::commandEOLOnResponceSequence,NUM_ELS(HKCommDefs::commandEOLOnResponceSequence));

            //check if ammount written matches to what was expected
            if (written != toWrite + NUM_ELS(HKCommDefs::commandEOLOnResponceSequence))
            {
                g_commState.setErrorState(
                    HKCommState::ESerialErrorType::serialErr_WriteFail);
            }
            {
                g_commState.setState(HKCommState::ESerialState::serialState_Preable);
            }
            return 1;
            break;
        }
        default:
        case HKCommState::ESerialState::serialState_Error:
        {
            HKCommState::ESerialErrorType  err  =  g_commState.getErrorType();
            uint8_t                        serr = g_commState.getErrSUBType();
            int64_t errorcode = 0;
            if (err == HKCommState::ESerialErrorType::serialErr_LogicNoSuchCommand)
            {
                errorcode = static_cast<uint32_t>(g_RecievedCmd.getCommand());
                errorcode *= 10000000;
            }
            //we will do more actions depending on type of g_serialError;
 
            errorcode += static_cast <uint8_t>(err);
            errorcode *= 1000;
            errorcode += serr;

            g_OutBuilder.reset();
            g_OutBuilder.putCMD("ERR");
            g_OutBuilder.addInt(errorcode);

            g_commState.setState(HKCommState::ESerialState::serialState_Respond);

            return 1;

            break;
        }

    }
    return 0;
}


