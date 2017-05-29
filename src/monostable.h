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

#ifndef HK_MONOSTABLE_H
#define HK_MONOSTABLE_H
#include "hk_node_utils.h"
#include "hk_time.h"
//--------------------------------------------------
/** 
Handled for monostable operations

The `switchToDefault` supposed to be set periodically
EVEN WHEN ALREADY IN DEFAULT STATE!

When setting up time for the executor it starts counting
If right after that value gets changed the monstable
operation would take place
*/
class Monostable
{
public:
    ENUM(ErrCodes)
    {
        ok,
    };
    static void switchToDefault(void); //!< Called periodically to set to bring back to default state
    static ErrCodes setValue(int32_t value); //!< Sets the value, will be brought to default with switchToDefault
    static ErrCodes setup (HKTime::SmallUpTime timeSec, int32_t newValue); //!<Sets up new value for given time
private:
    static int32_t getDefault();

};

//--------------------------------------------------



#endif

