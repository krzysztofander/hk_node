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
    if (g_commState.getState ()!= HKCommState::ESerialState::serialState_Preable
        || HKSerial::available() > 0)
        return 1;
    else
        return 0;
}
/*
void HKComm::jumpToAction(const uint8_t * command,const uint8_t * data, const uint16_t dataSize)
{
    g_serialError = HKCommDefs::serialErr_None;
    g_SerialState = HKCommDefs::serialState_Action;
    g_dataIt      = 0;

    g_command[0] = command[0];
    g_command[1] = command[1];
    g_command[2] = command[2];

    for (g_dataIt = 0; (g_dataIt < dataSize) && (g_dataIt < NUM_ELS(g_data)); g_dataIt++)
    {
        *g_data = *data;
    }

}

void HKComm::jumpToResp(const uint8_t * command, const  uint8_t * data, const uint16_t dataSize)
{
    jumpToAction(command, data, dataSize);
    g_SerialState = HKCommDefs::serialState_Respond;

}
*/


void HKComm::command_DED(OutBuilder & bld)
{
    bld.reset();
    bld.putCMD("DER");
    bld.putData(NULL, 0);
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
                        g_commState.setErrorState(
                            HKCommState::ESerialErrorType::serialErr_Parser,
                            parseResult  );

                     }
                    return true;
                }
            }
            break;
        }
        case HKCommState::ESerialState::serialState_Action:
        {
            //we leave builder is errored state
            switch (g_RecievedCmd.cmd)
            {
                case HKCommDefs::command_DED:
                {
                    command_DED(g_OutBuilder);
                }
                break;
                case HKCommDefs::command_RTH:
                {
                    uint16_t elems = g_RecievedCmd.getUint16(g_OutBuilder.err);
                    if (!g_OutBuilder.getError())
                    {
                        command_RTH(g_OutBuilder,elems);
                    }
                }
                break;
                default:
                {
                    g_OutBuilder.err = HKCommDefs::serialErr_UnknownCommand;
                    g_OutBuilder.putCMD("ERR");
                    g_OutBuilder.putInt(g_RecievedCmd.cmd);
                }
            }
            if (g_OutBuilder.getError())
            {
                g_serialError = HKCommDefs::serialErr_Logic + g_OutBuilder.getError();
            }

            if (g_serialError == HKCommDefs::serialErr_None)
            {
                g_SerialState = HKCommDefs::serialState_Respond;
            }
            else
            {
                g_SerialState = HKCommDefs::serialState_Error;
            }
            return true;
        }
        case HKCommState::ESerialState::serialState_Respond:
        {

            //write command then variable number of data then end of line sequence.
            uint16_t written = HKSerial::write(g_OutBuilder.inOutCommand, 3 /*todo fix it*/);
            if (g_OutBuilder.dataSize != 0)
            {
                written += HKSerial::write(g_OutBuilder.inOutData, g_OutBuilder.dataSize);
            }
            //write extra records if any
            bool valid = 0;
            do
            {
                g_serialError = HKCommExtraRecordsHDL::formatedMeasurement(valid,g_OutBuilder);
                if (g_serialError != HKCommDefs::serialErr_None)
                {
                    g_serialError  += HKCommDefs::serialErr_WriteFail;
                    g_SerialState = HKCommDefs::serialState_Error;
                    return 1;
                }
                if (!valid)
                {
                    written += HKSerial::write(g_OutBuilder.inOutData,  g_OutBuilder.dataSize);
                }
            } while (!valid);


            written += HKSerial::write(HKCommDefs::commandEOLOnResponceSequence,NUM_ELS(HKCommDefs::commandEOLOnResponceSequence));

            //check if ammount written matches to what was expected
            //if (written != NUM_ELS(g_command)+ g_dataIt + extraRecCharsCounter + NUM_ELS(HKCommDefs::commandEOLOnResponceSequence))
            //{
            //    g_serialError = HKCommDefs::serialErr_WriteFail;
            //    g_SerialState = HKCommDefs::serialState_Error;
            //    //leaving g_dataIt  as is for debug purposes...
            //}
            //else
            {
                g_serialError = HKCommDefs::serialErr_None;
                g_SerialState = HKCommDefs::serialState_Preable;
            }
            return 1;
            break;
        }
        default:
        case HKCommState::ESerialState::serialState_Error:
        {
            //we will do more actions depending on type of g_serialError;
            g_OutBuilder.putCMD("ERR");
            g_OutBuilder.putInt(g_serialError);

            g_SerialState = HKCommDefs::serialState_Respond;
            g_serialError = HKCommDefs::serialErr_None;
            return 1;

            break;
        }

    }
    return 0;
}


