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
#include "executor.h"
#include "serial.h"
#include "comm.h"
#include "supp.h"
#include "temp_measurement.h"

const uint8_t  HKComm::commandEOLOnResponceSequence[2] ={ '\n', '\r' }; //sequence send as an end of line on response

//@Brief parses the ASCII and fills the pointer with value
//@Returns 0 if ok, 
uint8_t charToUnsigned(uint8_t givenChar, uint8_t *valToSet)
{
    if (givenChar >= uint8_t('0') && givenChar <= uint8_t('9') )
    {
        *valToSet = givenChar - uint8_t('0');
        return 0;
    }
    else if (givenChar >= uint8_t('a') && givenChar <= uint8_t('f'))
    {
        givenChar -= (uint8_t('a') - uint8_t(10));
        *valToSet = givenChar;
        return uint8_t(0);
    }
    return uint8_t(1);
}

void HKComm::echoLetter(uint8_t l)
{
    uint8_t sequence[] = "0x?? \n\r";
    uint8_t highHex = l >> 4 ;
    highHex += highHex > uint8_t(9) ? uint8_t('a')  - uint8_t(10) : uint8_t('0');
    
    uint8_t lowHex = l & uint8_t(0xF);
    lowHex += lowHex > uint8_t(9) ? uint8_t('a') - uint8_t(10) : uint8_t('0');

    sequence[2] = highHex;
    sequence[3] = lowHex;
    if (l > uint8_t(' '))
    {
        HKSerial::write(&l,1); 
        uint8_t arrow[] = " -> ";
        HKSerial::write(arrow,4); 
        HKSerial::write(sequence, NUM_ELS(sequence)- 3); //no need for EOL
    }
    else
    {
        HKSerial::write(sequence, NUM_ELS(sequence)- 1); //no need for terminating null
    }  
}


//@Brief parses the ASCII and fills the pointer with value
//@Returns 0 if ok, serialErr_IncorrectNumberFormat  if error
uint8_t HKComm::dataToUnsignedShort(uint16_t offset, const uint8_t (&inData)[commandMaxDataSize], uint16_t & retVal )
{
    if (offset <= commandMaxDataSize - sizeof(uint16_t) * 2 - commandEOLSizeOnRecieve)
    {
        for (uint8_t i = 0; i < sizeof(uint16_t) * 2; i++)
        {
            uint8_t v;
            uint8_t r = charToUnsigned(inData[offset + i], &v);
            if (!!r)
            {
                return serialErr_IncorrectNumberFormat;
            }
            retVal <<= 4; //bits per digit
            retVal |= (uint16_t)v;
        }
        return 0;
    }
    else
    {
        return serialErr_Assert;
    }
}

uint8_t HKComm::shortToData(uint16_t & inOutOffset, uint8_t (&inData)[commandMaxDataSize], const uint16_t  inVal )
{
    if (inOutOffset <= commandMaxDataSize - sizeof(uint16_t) * 2 - commandEOLSizeOnRecieve)
    {
        for (uint8_t i = 0; i < sizeof(inVal); i++)
        {
            uint8_t l = uint8_t(inVal >> (8 - (8 * i)));
            uint8_t highHex = l >> 4 ;
            highHex += highHex > uint8_t(9) ? uint8_t('a') - uint8_t(10) : uint8_t('0');
            uint8_t lowHex = l & uint8_t(0xF);
            lowHex += lowHex > uint8_t(9) ? uint8_t('a') - uint8_t(10) : uint8_t('0');

            inData[inOutOffset++] = highHex;
            inData[inOutOffset++] = lowHex;
        }
        return 0;
    }
    else
    {
        return serialErr_Assert;
    }

}


//------------------------------------------------------------------
// @brief Response for D (debug) function  

uint8_t HKComm::command_D(uint8_t (&inOutCommand)[commandSize], uint8_t (&inOutData)[commandMaxDataSize], uint16_t & dataSize)
{
    (void)inOutData;

    switch (inOutCommand[command_subIdPos1])
    {
    case 'E': //echo 
        //Ignore inOutCommand[command_subIdPos2] 
        //Response:
        //inOutCommand[commandIdentifierPos] = 'D';//already 'D'
          inOutCommand[command_subIdPos1] = 'R';
        //inOutCommand[command_subIdPos2] = as is
        break;
    default:  //unknown 'D' command
        //Response:
        //inOutCommand[commandIdentifierPos] = 'D';//already 'D'
        inOutCommand[command_subIdPos1] = 'u';
        inOutCommand[command_subIdPos2] =  'n';
        break;
    }
    dataSize = 0;
    return serialErr_None;  //no data set...
    
}

//------------------------------------------------------------------
// @brief Response for C (configuration) request function  
// @Returns 0 ok, 1 or above - serial error

uint8_t HKComm::command_C(uint8_t (&inOutCommand)[commandSize], uint8_t (&inOutData)[commandMaxDataSize], uint16_t & dataSize)
{
    (void)inOutData;
    switch (inOutCommand[command_subIdPos1])
    {
    case 'T': //configure temperature
        if (inOutCommand[command_subIdPos2] = 'M')
        {
            //CTC

            //check for size correctness
            if (dataSize < sizeof(short) * 2)
            {
                return serialErr_NumberToShort;
            }

            uint16_t tempMeasmntInterval;
            uint8_t e = dataToUnsignedShort(0, inOutData, tempMeasmntInterval);

            if (e != serialErr_None)
            {
                return e;
            }

            Executor::setExecutionTime((uint8_t)Executor::temperatureMeasurer, (Sleeper::SleepTime)tempMeasmntInterval);
            //Response is same as command...
            //todo actually read that and respond accordingly

                //not touch data
                //set size to number of elements in data.
            dataSize = sizeof(short) * 2;
            return serialErr_None;
        }
        inOutCommand[command_subIdPos2] = 'u';
        dataSize = 0;
        dataSize = serialErr_None;
     
    default:  //unknown 'C' command
        //Response:
        inOutCommand[command_subIdPos1] =  'u';
        inOutCommand[command_subIdPos2] =  'n';
        dataSize = 0;
        return serialErr_None;
    }

}

uint8_t HKComm::command_R(uint8_t (&inOutCommand)[commandSize], uint8_t (&inOutData)[commandMaxDataSize], uint16_t & dataSize)
{
    uint8_t retVal;
    switch (inOutCommand[command_subIdPos1])
    {
    case 'T':

        switch (inOutCommand[command_subIdPos2])
        {
        case 'M':
        {
            //make a measurement
            TempMeasure::TempMeasurement val = TempMeasure::getSingleTempMeasurement();
            inOutCommand[commandIdentifierPos] =  'V';
            //inOutCommand[command_subIdPos1] =  'T';
            //inOutCommand[command_subIdPos2] =  'M'
            uint16_t it = 0;
            inOutData[it++]='(';
            retVal = shortToData(it, inOutData, 0);
            inOutData[it++]=',';
            retVal = shortToData(it, inOutData, val);
            inOutData[it++]=')';
            if (retVal != serialErr_None)
            {
                return retVal;
            }
            dataSize = it;
            break;
        }
        default:
        {
            dataSize = 0;
            break;
        }
        }
        break;
    default:  //unknown 'T' command
    {
              //Response:
              //inOutCommand[commandIdentifierPos] = 'D';//already 'D'
        inOutCommand[command_subIdPos1] = 'u';
        inOutCommand[command_subIdPos2] =  'n';
        break;
    }
    }
    return serialErr_None;  //no data set...
}



//------------------------------------------------------------------

uint8_t HKComm::g_SerialState = serialState_ReadCommand;
uint8_t HKComm::g_command[HKComm::commandSize];
uint8_t HKComm::g_data[HKComm::commandMaxDataSize];
uint16_t HKComm::g_dataIt = 0;

uint8_t HKComm::g_serialError = serialErr_None;

uint8_t HKComm::isActive(void)
{
    if (g_SerialState != serialState_ReadCommand
        || HKSerial::available() > 0)
        return 1;
    else
        return 0;
}

// @brief Main function responding to serial data
// @returns True if switched state and shall be called immediately.

uint8_t  HKComm::respondSerial(void)
{

    //alert(g_SerialState +1, false);
    switch (g_SerialState)
    {
        case serialState_ReadCommand:
        {
            // read whole command first.
            if (HKSerial::available() >=  NUM_ELS(g_command))
            {
                //peek for preamble:
                if (HKSerial::peek() == preamble)
                {
                    //ignore preamble
                    HKSerial::read();
                    return false;   // the upper condition may not be met. Returning to main loop
                }

                for (uint8_t commandIt = 0; commandIt  < NUM_ELS(g_command); commandIt ++)
                {
                    g_command[commandIt] = HKSerial::read();
                    //in case of error here...
                    if (g_command[commandIt] == uint8_t(commandEOLSignOnRecieve))
                    {
                        g_serialError = serialErr_eolInCommand;
                        g_SerialState =  serialState_Error;
                        return true;
                    }
                }
                g_serialError = serialErr_None;
                //all ok, switch to data
                g_SerialState = serialState_ReadData;
                g_dataIt = 0;
                return 1;
            }
            break;
        }

        case serialState_ReadData:
        {
            if (HKSerial::available() >= 1)
            {
                //read up to end of line or to error
                g_data[g_dataIt] = HKSerial::read();
                //echoLetter(g_data[g_dataIt] );
                if (g_data[g_dataIt] == uint8_t(commandEOLSignOnRecieve))
                {
                      //found end of line
                    g_SerialState =  serialState_Action;
                    return 1;
                }
                else
                {   
                    if (g_dataIt >= NUM_ELS(g_data) - 1 /*cannot increase g_dataIt as next read we would go out of buffer*/ )
                    {
                        //lost end of line and have a buffer full already. It must be an error
                        g_serialError = serialErr_noEolFound;
                        g_SerialState =  serialState_Error;
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
        case serialState_Action:
        {
            //command  and data recieved. Handle that...
            //TODO remove this state.
            
            switch (g_command[commandIdentifierPos])
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
                g_serialError  = serialErr_UnknownCommand;
            }

            if (g_serialError == serialErr_None)
            {
                //we have response command and response data in buffers. 

                g_SerialState = serialState_Respond;
            }
            else
            {
                g_SerialState = serialState_Error;
            }
            return 1;
            break;
        }
        case serialState_Respond:
        {
            
            //write command then variable number of data then end of line sequence.
            uint8_t written = HKSerial::write(g_command,NUM_ELS(g_command));
            if (g_dataIt != 0)
            {
                written += HKSerial::write(g_data, g_dataIt);
            }
            written += HKSerial::write(commandEOLOnResponceSequence,NUM_ELS(commandEOLOnResponceSequence));

            //check if ammount written matches to what was expected
            if (written != NUM_ELS(g_command)+ g_dataIt + NUM_ELS(commandEOLOnResponceSequence))
            {
                g_serialError = serialErr_WriteFail;
                g_SerialState = serialState_Error;
                //leaving g_dataIt  as is for debug purposes...
            }
            else
            {
                g_serialError = serialErr_None;
                g_SerialState = serialState_ReadCommand;
                g_dataIt = 0;
            }
            return 1;
            break;
        }
        default:
        case serialState_Error:
        {
            
            if (serialErr_WriteFail != g_serialError)
            {
                g_command[0]='E';
                g_command[1]='R';
                g_command[2]='R';


                g_dataIt = 0;
                shortToData(g_dataIt, g_data, g_serialError);

                g_SerialState = serialState_Respond;
                g_serialError = serialErr_None;
                return 1;
            }
            break;
        }

    }
    return 0;
}


