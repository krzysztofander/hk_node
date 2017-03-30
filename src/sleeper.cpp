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
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/io.h>
#include <avr/wdt.h>


#include "hk_node.h"
#include "sleeper.h"
#include "executor.h"
#include "serial.h"
#include "supp.h"
#include "comm.h"
//---------------------------------------------------------------
Sleeper::SleepTime Sleeper::g_sleepTime     = 0;
volatile HKTime::UpTime  Sleeper::g_upTime          = 0;
HKTime::UpTime Sleeper::g_lastUpTime               = 0;
volatile uint8_t Sleeper::gv_wdInterrupt = 0; //raised ones WD triggers an interrupt.
volatile uint8_t Sleeper::gv_wdInterrupt_B = 0; //raised ones WD triggers an interrupt.
volatile uint8_t Sleeper::gv_NoPowerDownPeriod = 0;   //!counts down WD ticks how long to stay awake 
 uint8_t Sleeper::g_NoPowerDownPeriodSetting = 1;   //!counts down WD ticks how long to stay awake 
Sleeper::PowerSaveMode Sleeper::g_PowerSaveMode = Sleeper::PowerSaveMedium;

//---------------------------------------------------------------
/* Author: Lim Phang Moh
* Company: Rocket Scream Electronics
* Website: www.rocketscream.com
*
* This is a lightweight low power library for Arduino.
*
* This library is licensed under Creative Commons Attribution-ShareAlike 3.0 
*/
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

void Sleeper::setNextSleep(Sleeper::SleepTime  st)
{
    g_sleepTime = st;
}
//---------------------------------------------------------------

Sleeper::SleepTime Sleeper::howMuchDidWeSleep(void)
{
    Sleeper::SleepTime retVal;
    HKTime::UpTime thisUpTime = getUpTime();
    if (((thisUpTime - g_lastUpTime) >> (sizeof(Sleeper::SleepTime) * 8)) != 0)
    {
        //did not fit or negative, return biggest possible
        retVal = 0x7FFFFFFF;
    }
    else
    {
        retVal = Sleeper::SleepTime(thisUpTime - g_lastUpTime);
    }
    g_lastUpTime = thisUpTime;
    return retVal;
}
//http://www.gammon.com.au/forum/?id=11497

//---------------------------------------------------------------

//@increases the system time by one
//This function should NOT be called by ISR 
void Sleeper::setTime(const volatile HKTime::UpTime newTime)
{
    do
    {
        Sleeper::g_upTime = newTime;
    } while (Sleeper::g_upTime != newTime);
}

//---------------------------------------------------------------


//@increases the system time by one
//This function should be called ONLY by ISR handling time ticks
void Sleeper::incUpTimeInISR(void)
{
    Sleeper::g_upTime++;
}

//---------------------------------------------------------------

// watchdog interrupt
ISR (WDT_vect) 
{
    Sleeper::gv_wdInterrupt  = 1;        //annotate that the interrupt came from watchdog.
    Sleeper::gv_wdInterrupt_B  = 1;      //annotate that the interrupt came from watchdog.

    Sleeper::incUpTimeInISR();
    
    if (Sleeper::gv_NoPowerDownPeriod > 0)
    {
        Sleeper::gv_NoPowerDownPeriod --;
    }
}  // end of WDT_vect

//---------------------------------------------------------------

//Pin interrupt. will be trigerred (or not...) from serial
//
//.. WARNING::
//The ISR may not be executed it AVR is in Power Down mode
//Despiate that the AVR gets woken op
ISR (PCINT2_vect)
{
   PCICR  &= ~bit (PCIE2); // disable pin change interrupts for D0 to D7
  
}
//---------------------------------------------------------------

HKTime::UpTime Sleeper::getUpTime(void)
{
    volatile  HKTime::UpTime tempTime1;
    volatile  HKTime::UpTime tempTime2;
    do
    {
        tempTime1 = Sleeper::g_upTime;
        tempTime2 = Sleeper::g_upTime;
    } while (tempTime1 != tempTime2);
    return tempTime1;
}
//---------------------------------------------------------------

void Sleeper::setWDScale(int8_t scale)
{
    noInterrupts ();  
        // allow changes, disable reset
        WDTCSR = bit (WDCE) | bit (WDE);
        // set interrupt mode and an interval 
        //WDTCSR = bit (WDIE) | bit (WDP3) | bit (WDP0);    // set WDIE, and 8 seconds delay
        WDTCSR = bit (WDIE) | bit (WDP2) | bit (WDP1) ;     // set WDIE, and 1 seconds delay
    
        g_upTime = 0U;
        g_lastUpTime  = 0U;    
    interrupts ();    

    wdt_reset();  // pat the dog

}
//---------------------------------------------------------------

int8_t Sleeper::getWDScale()
{
    return 6; //@todo read the value from registers
}
//---------------------------------------------------------------

void Sleeper::initWD(void)
{
    setWDScale(6); //@todo use the value from NV
}
//---------------------------------------------------------------

void Sleeper::init(void)
{
    MCUSR = 0;          //clear reset flag register

    Sleeper::initWD();
    EIMSK = 0;                              //disable Int0, int 1
    PCMSK2 = bit (PCINT16) | bit(PCINT19);  // want pin 0 and 3 ONLY
    //interrupts will be enabled just before sleep
    HKSerial::begin(9600);
}
//---------------------------------------------------------------

void Sleeper::goToSleep(void)
{
     
    HKTime::UpTime time = getUpTime();
    
    
    if (1 
        && gv_NoPowerDownPeriod == 0 //if we recetly came out of sleep not because of watchdog, loop until WD tick again.
        && g_sleepTime > 0          //we want to sleep
        && ! HKComm::isActive()     //serial does command processing now
        )
    {
        Supp::aboutToPowerDown();

        //finish off serial
        //NOTE: Apparently serial have to be ON to call flush or end. Otherwise locks here...
        HKSerial::flush();
        HKSerial::end();

        //Prepare for sleep

        PCIFR  &= ~bit (PCIF2);                 // clear any outstanding interrupts
        PCICR  |=  bit (PCIE2);                 // enable pin change interrupts for D0 to D7
        gv_wdInterrupt = 0;                     // clear the indication flag

        //select sleep mode depending on WD
        if (PowerSaveHigh == g_PowerSaveMode
           && gv_wdInterrupt_B != 0  /* last time it was watchdog that woke up, serial calm, so entering here*/)
        {
            Supp::powerSaveHigh();
        
            //total down
            //when waking up from powed down some characters get lost on serial input

           
            ADCSRA &= ~(1 << ADEN);          //Disable ADC
            lowPowerBodOff(SLEEP_MODE_PWR_DOWN);

           
        }
        else
        {
            if (    PowerSaveMedium == g_PowerSaveMode
                ||  PowerSaveHigh   == g_PowerSaveMode /*in last wake I was not from WD*/ 
                )
            {
                Supp::powerSaveMedium();

                ADCSRA &= ~(1 << ADEN);          //Disable ADC
                /* from AT mega spec:
                   "When the BOD has been disabled, the wake-up time from sleep 
                    mode will be approximately 60us to ensure that the BOD is 
                    working correctly before the MCU continues executing code."
                   So, an extra delay of 60us happens.
                   Because of this strange things happen on serial
                   The first character is malformed.
                   The solution would be to drop first char or to leave BOD ON
                */
                lowPowerBodOn(SLEEP_MODE_STANDBY);

            }
            else
            {
                Supp::powerSaveLow();
                set_sleep_mode (SLEEP_MODE_IDLE);

                sleep_enable();
                sleep_cpu ();  
                //SLEEPING HERE
                sleep_disable();

            }
        }
     

        if (gv_wdInterrupt == 0)
        {
            //interrupt did not came from watchdog as WD is setting this to 1
            //disable pin intterupts and fire off serial
            //this can and should be done reqardless whether ISR picked that 

            PCICR  &= ~bit (PCIE2);     // disable pin change interrupts for D0 to D7
            
            //Serial.begin(9600);         //@TODO investigate whether is better to use that...
                                        //       UCSR0B |= bit (RXEN0);  // enable receiver
                                        //       UCSR0B |= bit (TXEN0);  // enable transmitter
            UCSR0B |= bit (RXEN0) | bit (TXEN0) |  bit(RXCIE0);


            //we need to introduce a delay here
            //in case when serial woken up, the char would get lost
            //and we would enter this loop immeditely
            //to avoid it we just hold up with sleeping for 
            //the time to >2 chars to arrive
            //in 9600 we have apx 0.1ms per char so wait of 3 ms would be fine
            //for terminal, manual operations,
            //a 50ms should do
            delay(50);

            gv_wdInterrupt_B  = 0;       //WARNING: This flag is cleared only here....
                                         //Used for HIGH power donw, delayin it for one tick     
            
            do
            {
                gv_NoPowerDownPeriod = g_NoPowerDownPeriodSetting;
            } while (gv_NoPowerDownPeriod != g_NoPowerDownPeriodSetting);
            Supp::notWDWakeUp();
        }
        else
        {
            //it was from watchdog. Clear the indication flag so it gets set again in WD ISR
            gv_wdInterrupt = 0;         //WARNING: This flag is cleared only here....

            HKSerial::begin(9600);         //@TODO investigate whether is better to use that...
                                        //       UCSR0B |= bit (RXEN0);  // enable receiver
                                        //       UCSR0B |= bit (TXEN0);  // enable transmitter
            Supp::watchdogWakeUp();
        }
        Supp::justPoweredUp();
    }
    else
    {
        Supp::noPowerDownHappened();
        //we do not go to sleep. The tick will return some time passed eventually
    }
}

void Sleeper::setNoPowerDownPeriod(uint8_t noPowerDownTicks)
{
    g_NoPowerDownPeriodSetting  = noPowerDownTicks;
}

uint8_t Sleeper::getNoPowerDownPeriod()
{
    return g_NoPowerDownPeriodSetting;
}

void Sleeper::setPowerSaveMode(Sleeper::PowerSaveMode powerSaveMode)
{
    g_PowerSaveMode = powerSaveMode;
}

uint8_t Sleeper::getPowerSaveMode()
{
    return g_PowerSaveMode;
}
