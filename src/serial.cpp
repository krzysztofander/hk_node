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

//------------------------------------------------------------------
void respondSerial(void)
{
    static char command[4];
    static char commandIt = 0;
    
    
    
    while (Serial.available() > 0)
    {
        alert(AlertReason_serialChar);
        command[commandIt++] = Serial.read();
        if (commandIt >= NUM_ELS(command))
        {
            break;
        }
    }
    if (commandIt >= NUM_ELS(command))
    {
        //have full command
        commandIt = 0;
        unsigned char written = 0;

        if (command[0] == 'D')
        {
            if (command[1] == 'E')
            {
                //echo 
                command[0] = 'D';
                command[1] = 'R';
                //command[2] = as is
                //command[3] = as is (end of line expected...D
            }
            else
            {
                command[0] = 'D';
                command[1] = 'u';
                command[2] = 'n';
                //command[3] = as is (end of line expected...

            }
        }
        else
        {
            command[0] = 'U';
            command[1] = 'N';
            command[2] = 'K';
            command[3] = '\n';
        }
        alert(AlertReason_serialSend);
        written = Serial.write(command,NUM_ELS(command));
    }
}


