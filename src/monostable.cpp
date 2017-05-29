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
#include "supp.h"
#include "monostable.h"
#include "executor.h"

//------------------------------------------------------------------
void Monostable::switchToDefault(void)
{
    setValue(getDefault());
    //actually here we could disable the executor.
    //but we leave it running anyway
}
    
Monostable::ErrCodes Monostable::setValue(int32_t value)
{
    return Monostable::ErrCodes::ok;
}

int32_t Monostable::getDefault()
{
    return 0;
}
Monostable::ErrCodes Monostable::setup(HKTime::SmallUpTime timeSec, int32_t newValue)
{
    uint8_t myExecutor = Executor::monostable1;
    if (! Executor::isExecutorActive(myExecutor))
    {
        Executor::setupExecutingFn(myExecutor,
                                   timeSec,
                                   Monostable::switchToDefault);
        //Normally the setupExecutingFn causes the executor to be called
        //in the next main loop 
        //This is why the setExecutionTime is called regardless.
    }
    Executor::setExecutionTime(myExecutor, timeSec);
    
    //ones we have schedule to bringing it back to default
    //we can set the default value
    setValue(newValue);

}



//------------------------------------------------------------------
