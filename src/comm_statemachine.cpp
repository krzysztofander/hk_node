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

uint8_t  HKComm::g_SerialState = HKCommDefs::serialState_ReadCommand;
uint8_t  HKComm::g_command[HKCommDefs::commandSize];
uint8_t  HKComm::g_data[HKCommDefs::commandMaxDataSize];
uint16_t HKComm::g_dataIt = 0;
uint8_t  HKComm::g_serialError = HKCommDefs::serialErr_None;


//------------------------------------------------------------------

uint8_t HKComm::isActive(void)
{
    if (g_SerialState != HKCommDefs::serialState_ReadCommand
        || HKSerial::available() > 0)
        return 1;
    else
        return 0;
}

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



// @brief Main function responding to serial data
// @returns True if switched state and shall be called immediately.

uint8_t  HKComm::respondSerial(void)
{

    //alert(g_SerialState +1, false);
    switch (g_SerialState)
    {
        case HKCommDefs::serialState_ReadCommand:
        {
            // read whole command first.
            if (HKSerial::available() >=  NUM_ELS(g_command))
            {
                //peek for preamble:
                if (HKSerial::peek() == HKCommDefs::preamble)
                {
                    //ignore preamble
                    HKSerial::read();
                    return false;   // the upper condition may not be met. Returning to main loop
                }
                g_serialError = HKCommDefs::serialErr_None;
                g_SerialState = HKCommDefs::serialState_ReadData;
                g_dataIt      = 0;

                for (uint8_t commandIt = 0; commandIt  < NUM_ELS(g_command); commandIt ++)
                {
                    g_command[commandIt] = HKSerial::read();
                    //in case of error here
                    if (g_command[commandIt] == uint8_t(HKCommDefs::commandEOLSignOnRecieve))
                    {
                        g_serialError = HKCommDefs::serialErr_eolInCommand;
                        g_SerialState =  HKCommDefs::serialState_Error;
                        //loop will continue reading whole command (3 chars) anyway.
                    }
                }
                return 1;
            }
            
            break;
        }

        case HKCommDefs::serialState_ReadData:
        {
            if (HKSerial::available() >= 1)
            {
                //read up to end of line or to error
                g_data[g_dataIt] = HKSerial::read();
                //echoLetter(g_data[g_dataIt] );
                if (g_data[g_dataIt] == uint8_t(HKCommDefs::commandEOLSignOnRecieve))
                {
                      //found end of line
                    g_SerialState =  HKCommDefs::serialState_Action;
                    return 1;
                }
                else
                {   
                    if (g_dataIt >= NUM_ELS(g_data) - 1 /*cannot increase g_dataIt as next read we would go out of buffer*/ )
                    {
                        //lost end of line and have a buffer full already. It must be an error
                        g_serialError = HKCommDefs::serialErr_noEolFound;
                        g_SerialState =  HKCommDefs::serialState_Error;
                        //terminating data with EOL anyway
                        g_data[NUM_ELS(g_data) - 1] = uint8_t(HKCommDefs::commandEOLSignOnRecieve);
                        return 1;
                    }
                    else
                    {
                        //still place and did not recieve the end of line yet. Keep gathering
                        g_dataIt++;
                        
                        return 1; //maybe the is more data, so try immediately
                    }
                }
            }
            break;
        }
        case HKCommDefs::serialState_Action:
        {
            //command  and data recieved. Handle that...
            //TODO remove this state.
            
            switch (g_command[HKCommDefs::commandIdentifierPos])
            {
            case 'D':
                g_serialError = command_D(g_command, g_data, g_dataIt);
                break;
            case 'C':
                g_serialError = command_C(g_command, g_data, g_dataIt);
                break;
            case 'R':
                g_serialError = command_R(g_command, g_data, g_dataIt);
                break;

            default :
                g_serialError  = HKCommDefs::serialErr_UnknownCommand;
            }

            if (g_serialError == HKCommDefs::serialErr_None)
            {
                //we have response command and response data in buffers. 

                g_SerialState = HKCommDefs::serialState_Respond;
            }
            else
            {
                g_SerialState = HKCommDefs::serialState_Error;
            }
            return 1;
            break;
        }
        case HKCommDefs::serialState_Respond:
        {

            //write command then variable number of data then end of line sequence.
            uint16_t written = HKSerial::write(g_command, NUM_ELS(g_command));
            if (g_dataIt != 0)
            {
                written += HKSerial::write(g_data, g_dataIt);
            }
            //write extra records if any
            uint8_t valid = 0;
            uint8_t err = 0;
            uint16_t extraRecChars;
            uint16_t extraRecCharsCounter = 0;
            do
            {
                extraRecChars  =0;
                g_serialError = HKCommExtraRecordsHDL::formatedMeasurement(valid, extraRecChars, g_data);
                if (g_serialError != HKCommDefs::serialErr_None)
                {
                    g_SerialState = HKCommDefs::serialState_Error;
                    return 1;
                }
                if (valid != 0)
                {
                    written += HKSerial::write(g_data, g_dataIt);
                    extraRecCharsCounter += extraRecChars;
                }

            } while (valid != 0);


            written += HKSerial::write(HKCommDefs::commandEOLOnResponceSequence,NUM_ELS(HKCommDefs::commandEOLOnResponceSequence));

            //check if ammount written matches to what was expected
            if (written != NUM_ELS(g_command)+ g_dataIt + extraRecCharsCounter + NUM_ELS(HKCommDefs::commandEOLOnResponceSequence))
            {
                g_serialError = HKCommDefs::serialErr_WriteFail;
                g_SerialState = HKCommDefs::serialState_Error;
                //leaving g_dataIt  as is for debug purposes...
            }
            else
            {
                g_serialError = HKCommDefs::serialErr_None;
                g_SerialState = HKCommDefs::serialState_ReadCommand;
                g_dataIt = 0;
            }
            return 1;
            break;
        }
        default:
        case HKCommDefs::serialState_Error:
        {
            g_dataIt = 0;
            g_data[g_dataIt++] = ' ';
            for (uint8_t i = 0; i < NUM_ELS(g_command); i++)
            {
                if (g_command[i] > uint8_t (' ') && g_command[i] < 127)
                {
                    g_data[g_dataIt++] = g_command[i];
                }
                else
                {
                    g_data[g_dataIt++] = '\\';
                    HKCommCommon::uint8ToData(g_dataIt, g_data, g_command[i]);
                }
            }

            g_data[g_dataIt++] = '-';
            if ((uint8_t)HKCommDefs::serialErr_WriteFail == g_serialError)
            {
                //lets attempt to do something anyway...
                HKSerial::flush();
                HKSerial::end();
                HKSerial::begin(9600);
            }

            g_command[0]='E';
            g_command[1]='R';
            g_command[2]='R';

          
            HKCommCommon::shortToData(g_dataIt, g_data, g_serialError);
            

            g_SerialState = HKCommDefs::serialState_Respond;
            g_serialError = HKCommDefs::serialErr_None;
            return 1;

            break;
        }

    }
    return 0;
}


