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
#include <Arduino.h>

extern const char  commandEOLOnResponceSequence[2] ={ '\n', '\r' }; //sequence send as an end of line on response


void readData(char count, char (&inOutData)[commandMaxDataSize])
{
    char dataIt = 0;
    while (Serial.available() > 0 && dataIt < count)
    {
        inOutData[dataIt++] = Serial.read();
      
        //todo - remove that:
        alert(AlertReason_serialChar,true);
    }
}

void consume (char (&inOutData)[commandMaxDataSize])
{
    readData(0, inOutData);
}

//------------------------------------------------------------------
// @brief Response for D (debug) function  

inline char command_D(char (&inOutCommand)[commandSize], char (&inOutData)[commandMaxDataSize])
{
    (void)inOutData;

    switch (inOutCommand[command_subIdPos1])
    {
    case 'E': //echo 
       
        //Response:
        //inOutCommand[commandIdentifierPos] = 'D';//already 'D'
          inOutCommand[command_subIdPos1] = 'R';
        //inOutCommand[command_subIdPos2] = as is
        consume(commandEOLSizeOnRecieve);
        break;
    default:  //unknown 'D' command
        
        //Response:

        //inOutCommand[commandIdentifierPos] = 'D';//already 'D'
        inOutCommand[command_subIdPos1] = 'u';
        inOutCommand[command_subIdPos2] =  'n';
        consume(commandEOLSizeOnRecieve);
        break;
    }
    return 0;  //no data set...
    
}

//------------------------------------------------------------------
// @brief Response for C (configuration) request function  

inline char command_C(char (&inOutCommand)[commandSize], char (&inOutData)[commandMaxDataSize])
{
    (void)inOutData;

    switch (inOutCommand[command_subIdPos1])
    {
    case 'T': //configure temperature
        
        if (inOutCommand[command_subIdPos2] = 'T')
        {
            //CTC
            //Expecting setting in uint16
            
            //Response:
              //inOutCommand[commandIdentifierPos] = 'D';//already 'D'
        inOutCommand[command_subIdPos1] = 'R';
        //inOutCommand[command_subIdPos2] = as is
        consume(commandEOLSizeOnRecieve);
        break;
    default:  //unknown 'D' command

              //Response:

              //inOutCommand[commandIdentifierPos] = 'D';//already 'D'
        inOutCommand[command_subIdPos1] = 'u';
        inOutCommand[command_subIdPos2] =  'n';
        consume(commandEOLSizeOnRecieve);
        break;
    }
    return 0;  //no data set...

}


//------------------------------------------------------------------
// @brief Main function responding to serial data
void respondSerial(void)
{
    
    static char command[commandSize];
    static char commandIt = 0;
    static char data[commandMaxDataSize + commandEOLSizeOnRecieve];
    static char dataIt = 0;

    
    while (Serial.available() > 0)
    {
        //todo - remove that:
        alert(AlertReason_serialChar,true);
        command[commandIt++] = Serial.read();
        if (commandIt >= NUM_ELS(command))
        {
            break;
        }
    }

    
    //do not do anything untill we got full command
    if (commandIt >= NUM_ELS(command))
    {
        //have full command
        commandIt = 0;
        char written = 0;

        

        switch (command[commandIdentifierPos])
        {
        case 'D':
            dataIt = command_D(command, data);
            break;
        default :
            command[commandIdentifierPos]   = 'U';
            command[command_subIdPos1] = 'N';
            command[command_subIdPos2] = 'K';
            dataIt = 0;
        }
        
        //todo - remove that:
        alert(AlertReason_serialSend, false);
       
        //write command then variable number of data then end of line sequence.
        written = Serial.write(command,NUM_ELS(command));
        if (dataIt)
        {
            written += Serial.write(data, dataIt);
        }
        written = Serial.write(commandEOLOnResponceSequence,NUM_ELS(commandEOLOnResponceSequence));

        //check
        if (written != NUM_ELS(command)+ dataIt + commandEOLOnResponceSequence)
        {
            alert(AlertReason_serialwriteProblem, true);
        }

    }
}


