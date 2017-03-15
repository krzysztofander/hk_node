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

#ifndef HK_SLEEPER_H
#define HK_SLEEPER_H

class Sleeper
{
public:
    typedef int32_t SleepTime;  //signed

    //@sets next sleep time. 
    //N.B. It is not used right now as the device will sleep to next WD kick anyway
    static void setNextSleep(SleepTime  st);

    //@actually returns the time slept
    //wake up may be from watchdog or something else
    //in a later case it will likely return 0
    static SleepTime howMuchDidWeSleep(void);   //has a state...

    //@puts device to sleep if next sleep time is    
    static void goToSleep(void);

    static void init(void);

    //@returns current up time
    static HKTime::UpTime getUpTime(void);
    
    //@Increments the up time. Should be called in WD's ISR only
    static void incUpTimeInISR(void);
    
    //@Sets up time to sme specific value
    static void setTime(const volatile HKTime::UpTime newTime);

    //@Sets no power down time (after wake up from serial or button)
    static void setNoPowerDownPeriod(uint8_t noPowerDownTicks);
    //@Gets no power down time (after wake up from serial or button)
    static uint8_t getNoPowerDownPeriod();

    enum PowerSaveMode
    {
        PowerSaveLow = 0,
        PowerSaveMedium = 1,
        PowerSaveHigh =2
    };

    //@Set power down mode
    static void setPowerSaveMode(PowerSaveMode powerSaveMode);

    static uint8_t getPowerSaveMode();
private:

    static void initWD(void);
    //@Sets the WD tick time
    static void setWDScale(int8_t scale);

    //@gets the WD tick time 
    static int8_t getWDScale();

private:
    static SleepTime g_sleepTime;                   //!time for next sleep
    static volatile HKTime::UpTime g_upTime ;       //!system up time 
    static HKTime::UpTime          g_lastUpTime ;   //!helper variable used to calculate how much we slept
public:    
    static volatile uint8_t gv_wdInterrupt;         //!indicates that WD happened
    static volatile uint8_t gv_wdInterrupt_B;       //!indicates that WD happened
    static uint8_t g_NoPowerDownPeriodSetting;   //!Setting for how long to stay awake 
    static volatile uint8_t gv_NoPowerDownPeriod;   //!counts down WD ticks how long to stay awake 
    static PowerSaveMode g_PowerSaveMode;
};
//--------------------------------------------------


#endif

