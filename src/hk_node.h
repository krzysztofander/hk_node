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

#ifndef HK_NODE_H
#define HK_NODE_H
#include "Arduino.h"


//Todo move that to separate file
class HKTime
{
public:
    typedef int64_t UpTime;             //signed!
    //typedef uint32_t TimeDiff;
    typedef int32_t SmallUpTime;
    typedef int16_t ShortTimeDiff;      //signed!


    //@brief returns time difference in ShortTimeDiff (int16_t)
    //In case the actual difference is higher returns it saturated up to ShortTimeDiff range
    static ShortTimeDiff getShortDiff(const UpTime & current, const UpTime & last)
    {
        UpTime diff = current - last;
        if (diff >= 0)
        {
            if ((diff >> (sizeof(ShortTimeDiff) * 8)) != 0)
            {
                //does not fit
                return 0x7FFF;
            }
            else
            {
                return ShortTimeDiff(diff);  //will truncate MSB which are 0 anyway
            }
        }
        else
        {
           if (diff <= UpTime( -0x8000 ) )
           {
                 //does not fit
                 return int16_t(-0x8000);
           }
           else
           {
                return ShortTimeDiff(diff);  //will truncate MSB which are 1s anyway
           }
        }
    }

};


void setupBody();
void loopBody();

//--------------------------------------------------
//@TODO Move that to executor
typedef void (*ExecutingFn)(void);

void initAllFunctions(void);
void setupDoWhatYouShouldTab(void);
void doWhatYouShould(void);

//--------------------------------------------------



#endif

