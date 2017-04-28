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
#include "hk_node_utils.h"
#include "hk_node.h"

#if defined __AVR_ATmega328P__

#ifndef sleep_bod_disable
//Comms probably from arduino\hardware\tools\avr\avr\include\avr\sleep.h
#define sleep_bod_disable() 										\
do { 																\
  unsigned char tempreg; 											\
  __asm__ __volatile__("in %[tempreg], %[mcucr]" "\n\t" 			\
                       "ori %[tempreg], %[bods_bodse]" "\n\t" 		\
                       "out %[mcucr], %[tempreg]" "\n\t" 			\
                       "andi %[tempreg], %[not_bodse]" "\n\t" 		\
                       "out %[mcucr], %[tempreg]" 					\
                       : [tempreg] "=&d" (tempreg) 					\
                       : [mcucr] "I" _SFR_IO_ADDR(MCUCR), 			\
                         [bods_bodse] "i" (_BV(BODS) | _BV(BODSE)), \
                         [not_bodse] "i" (~_BV(BODSE))); 			\
} while (0)
#endif
#define	lowPowerBodOff(mode)\
do { 						\
      set_sleep_mode(mode); \
      cli();				\
      sleep_enable();		\
			sleep_bod_disable(); \
      sei();				\
      sleep_cpu();			\
      sleep_disable();		\
      sei();				\
} while (0);

#define	lowPowerBodOn(mode)	\
do { 						\
      set_sleep_mode(mode); \
      cli();				\
      sleep_enable();		\
      sei();				\
      sleep_cpu();			\
      sleep_disable();		\
      sei();				\
} while (0);

#endif


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

    ENUM ( PowerSaveMode )
    {
        low = 0,
        medium = 1,
        high = 2
    };

    //@Set power save mode
    static void setPowerSaveMode(PowerSaveMode powerSaveMode);

    static PowerSaveMode getPowerSaveMode();
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

