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


#include "hk_node_config.h"
#include "serial.h"
#include "comm.h"
#include "supp.h"
#include "comm_extra_records.h"
#include "comm_extra_rec_handlers.h"
#include "MiniInParser.h"


HKCommState      HKComm::g_commState;    
InCommandWrap    HKComm::g_RecievedCmd;  
OutBuilder       HKComm::g_OutBuilder;   

//------------------------------------------------------------------

bool HKComm::isActive(void)
{
    if (g_commState.getState () != HKCommState::ESerialState::serialState_Preable
        || HKSerial::isActive() )
        return true;
    else
        return false;
}

void HKComm::jumpToAction()
{
    g_commState.setState(HKCommState::ESerialState::serialState_Action);
}

void HKComm::jumpToResp()
{
    g_commState.setState(HKCommState::ESerialState::serialState_Respond);
}

//action state handler. 
//It could be inlined. It is a separate function for readability
void HKComm::actionState(void)
{
    //clear any error flags:
    g_OutBuilder.reset();
    //by default respond with same command:
    g_OutBuilder.putCMD(g_RecievedCmd.getCommand());
    switch (g_RecievedCmd.getCommand())
    {
        case ECommands::command_DER:
            command_DER(g_OutBuilder);
            break;
        case ECommands::command_RTH:
            command_RTH(g_RecievedCmd, g_OutBuilder);
            break;
        case ECommands::command_RTM:
            command_RTM(g_OutBuilder);
            break;
        case ECommands::command_RVI:
            //fall through
        case ECommands::command_AVI:
            command_AVI(g_OutBuilder);
            break;
#if HAVE_HUMAN_READABLE
        case ECommands::command_AHR:
            command_AHR(g_OutBuilder);
            break;
#endif
        case  ECommands::command_CTP:
            commandCTP(g_RecievedCmd, g_OutBuilder);
            break;
        case  ECommands::command_CBP:
            commandCBP(g_RecievedCmd, g_OutBuilder);
            break;
        case  ECommands::command_CBS:
            commandCBS(g_RecievedCmd, g_OutBuilder);
            break;
        case  ECommands::command_CST:
            commandCST(g_RecievedCmd, g_OutBuilder);
            break;
        case  ECommands::command_CSC:
            commandCSC(g_RecievedCmd, g_OutBuilder);
            break;
        case  ECommands::command_CSM:
            commandCSM(g_RecievedCmd, g_OutBuilder);
            break;
        case  ECommands::command_CSA:
            commandCSA(g_RecievedCmd, g_OutBuilder);
            break;
        case  ECommands::command_CNN:
            commandCNN(g_RecievedCmd, g_OutBuilder);
            break;
        case  ECommands::command_DLS:
            commandDLS(g_RecievedCmd, g_OutBuilder);
            break;
        case  ECommands::command_RPM:
            commandRPM(g_RecievedCmd, g_OutBuilder);
            break;
        case  ECommands::command_CRV:
            commandCRV(g_RecievedCmd, g_OutBuilder);
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
}

// @brief Main function responding to serial data
// @returns True if switched state and shall be called immediately.
bool  HKComm::respondSerial(void)
{
    switch (g_commState.getState())
    {
        case HKCommState::ESerialState::serialState_Preable:
            HKSerial::activate();
            
            if (HKSerial::preambleRecieved() )
            {
                g_RecievedCmd.reset();
                g_commState.setState(HKCommState::ESerialState::serialState_ParseCommand);
                return true;
            }
            return false;
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
                        g_commState.setErrorState(parseResult);
                    }
                    return true;
                }
            }
            return false;
        }
        case HKCommState::ESerialState::serialState_Action:
        {
            actionState();
            return true;
        }
        case HKCommState::ESerialState::serialState_Respond:
        {

            //write command then variable number of data then end of line sequence.
            uint16_t toWrite =  g_OutBuilder.getStrLenght();
            uint16_t written = HKSerial::write(
                g_OutBuilder.getStrToWrite(), g_OutBuilder.getStrLenght());
          
             //write extra records if any
            bool valid = true;
            while (valid && toWrite == written )
            {   
                g_OutBuilder.reset();
                auto ret = HKCommExtraRecordsHDL::formatedMeasurement(valid,g_OutBuilder);
                if (ret == HKCommExtraRecordsHDL::exitCodes::noMoreData)
                {
                    valid = false;
                }
                else if (ret ==  HKCommExtraRecordsHDL::exitCodes::ok)
                {
                    if (valid)
                    {
                        toWrite += g_OutBuilder.getStrLenght();
                        written += HKSerial::write(g_OutBuilder.getStrToWrite(), g_OutBuilder.getStrLenght());
                    }
                }
                else// if (ret == HKCommExtraRecordsHDL::exitCodes::generalError)
                {
                    g_commState.setErrorState(
                        HKCommState::ESerialErrorType::serialErr_WriteFail );
                    return true;
                }
            };

            toWrite += NUM_ELS(HKCommDefs::commandEOLOnResponceSequence);
            written += HKSerial::write(HKCommDefs::commandEOLOnResponceSequence,NUM_ELS(HKCommDefs::commandEOLOnResponceSequence));

            //check if ammount written matches to what was expected
            if (written != toWrite)
            {
                g_commState.setErrorState(
                    HKCommState::ESerialErrorType::serialErr_WriteFail);
            }
            else
            {
                //all fine, response was send back. To the beginning
                HKSerial::commandProcessed();
                g_commState.setState(HKCommState::ESerialState::serialState_Preable);
            }
            return true;
        }
        default:
        case HKCommState::ESerialState::serialState_Error:
        {
            HKCommState::ESerialErrorType  err    = g_commState.getErrorType();
            uint8_t                        subErr = g_commState.getErrSUBType();
            int64_t  errorcode ;
            uint32_t lastCmd;
 
            lastCmd = static_cast<uint32_t>(g_RecievedCmd.getCommand());
    
            //constructing error response
            g_OutBuilder.reset();
            g_OutBuilder.putCMD(ECommands::command_ERR);
            g_OutBuilder.addData(" code:",6);
            
            //Major error code is multiplied by 1000
            errorcode =  static_cast <uint8_t>(err);
            errorcode *= 1000;

            //Sub type is added on top
            errorcode += subErr;

            g_OutBuilder.addInt(errorcode);

            //Some extra info after errorcode
            g_OutBuilder.addData(",lstcmd:",8);

            //3 letter command, or whatever was seen as command
            for (int8_t i =2; i >=0 ; i--)
            {
                uint8_t c = (char)(lastCmd >> (i * 8) );
                if (c < uint8_t(' ') || c > 127)
                {
                    g_OutBuilder.addData("\\",1);
                    g_OutBuilder.addInt(c);
                }
                else
                {
                    g_OutBuilder.addData((char * )(&c),1);
                }
            }

            //all we can do is just send it. State is set to respond.
            miniInParserReset();
            //TODO: when the error was serialErr_WriteFail attempt to reset serial up front
            g_commState.setState(HKCommState::ESerialState::serialState_Respond);

            return true;
        }

    }
    return false;
}


