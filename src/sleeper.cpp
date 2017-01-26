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
uint8_t            Sleeper::scale           = 6;
volatile HKTime::UpTime  Sleeper::g_upTime          = 0;
HKTime::UpTime Sleeper::g_lastUpTime               = 0;
volatile uint8_t Sleeper::gv_wdInterrupt = 0; //raised ones WD triggers an interrupt.
volatile uint8_t Sleeper::gv_wdInterrupt_B = 0; //raised ones WD triggers an interrupt.
//---------------------------------------------------------------

void Sleeper::setNextSleep(Sleeper::SleepTime  st)
{
    g_sleepTime = st;
}
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


void Sleeper::incUpTime(void)
{
    Sleeper::g_upTime++;
}

// watchdog interrupt
ISR (WDT_vect) 
{
    Sleeper::gv_wdInterrupt  = 1;        //annotate that the interrupt came from watchdog.
    Sleeper::gv_wdInterrupt_B  = 1;      //annotate that the interrupt came from watchdog.

    Sleeper::incUpTime();

}  // end of WDT_vect


//Pin interrupt. will be trigerred (or not...) from serial
//
//.. WARNING::
//The ISR may not be executed it AVR is in Power Down mode
//Despiate that the AVR gets woken op
ISR (PCINT2_vect)
{
   PCICR  &= ~bit (PCIE2); // disable pin change interrupts for D0 to D7
   Serial.begin(9600);     // start serial
                           // not nessesarily here and in main loop.
  
}



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

void Sleeper::setWDScale(int8_t scale)
{
    noInterrupts ();  
        // allow changes, disable reset
        WDTCSR = bit (WDCE) | bit (WDE);
        // set interrupt mode and an interval 
        //WDTCSR = bit (WDIE) | bit (WDP3) | bit (WDP0);    // set WDIE, and 8 seconds delay
       // WDTCSR = bit (WDIE) | bit (WDP3); 
        WDTCSR = bit (WDIE) | bit (WDP2) | bit (WDP1) ;    // set WDIE, and 1 seconds delay
    
        g_upTime = 0U;
        g_lastUpTime  = 0U;    
    interrupts ();    

    wdt_reset();  // pat the dog

}

int8_t Sleeper::getWDScale(int8_t scale)
{
    return 6;
}


void Sleeper::initWD(void)
{
    
    setWDScale(6);

}

void Sleeper::init(void)
{


    Sleeper::initWD();
    EIMSK = 0;                              //disable Int0, int 1
    PCMSK2 = bit (PCINT16) | bit(PCINT19);  // want pin 0 and 3 ONLY
    //interrupts will be enabled just before sleep
}

void Sleeper::gotToSleep(void)
{
   
   
   //     HKComm::echoLetter('E');
   //     HKComm::echoLetter(0);
   //     HKComm::echoLetter(PCMSK2);
   //     HKComm::echoLetter(PCIFR);
   ///     HKComm::echoLetter(PCICR);
   //     HKComm::echoLetter(Sleeper::gv_wdInterrupt);
   //     HKComm::echoLetter(Sleeper::gv_wdInterrupt_B);
 //   blueOn();
    HKTime::UpTime time = getUpTime();
    alert(uint8_t(time & 0xF), false);
   // HKComm::echoLetter('A');
   //  HKComm::echoLetter(g_sleepTime & 0xFF);
    
    if (1 
     //   && gv_wdInterrupt_B != 0    //if we recetly came out of sleep not because of watchdog, loop until WD tick again.
        && g_sleepTime > 0          //we want to sleep
        && ! HKComm::isActive()     //serial does command processing now
        )
    {
        //finish off serial
        //NOTE: Apparently serial have to be on to call flush or end. Otherwise locks here...
        Serial.flush();
        Serial.end();

        //select sleep mode depending on WD
        if (
          0 //disabled this for now
           && gv_wdInterrupt_B != 0  /* && see extra node in else */)
        {
            //last time it was watchdog that woke up
            //total down
            blueOff();
            set_sleep_mode (SLEEP_MODE_PWR_DOWN);  

            //when waking up from powed down some characters 

            //other periph could be powered donw here.
        }
        else
        {
            
            //serial or button. Standby for now
            set_sleep_mode (SLEEP_MODE_STANDBY);  

            /*if we 1 tick from action the periph could be woken up here*/

        }
        
        //set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
        
        //set_sleep_mode(SLEEP_MODE_IDLE);
        digitalWrite(LED_BUILTIN, 0);

        // clear various "reset" flags
        MCUSR = 0;    

        PCIFR  &= ~bit (PCIF2);                 // clear any outstanding interrupts
  
        PCICR  |= bit (PCIE2);                  // enable pin change interrupts for D0 to D7
        gv_wdInterrupt = 0;                     // clear the indication flag

        sleep_enable();

  
            // turn off brown-out enable in software
            //MCUCR = bit (BODS) | bit (BODSE);
            //MCUCR = bit (BODS); 
        sleep_cpu ();  
        //SLEEPING HERE
        // ......
        //GOT SOME wake

        // cancel sleep as a precaution
        sleep_disable();

        if (gv_wdInterrupt == 0)
        {
            //interrupt did not came from watchdog as WD is setting this to 1
            //disable pin intterupts and fire off serial
            //this can and should be done reqardless whether ISR picked that 

            PCICR  &= ~bit (PCIE2); // disable pin change interrupts for D0 to D7
          
            
            //periph should be woken up here

            //WARNING: This flag is cleared only here....
            gv_wdInterrupt_B  = 0; // say to this function not to enter sleep before next WD tick      
        }
        else
        {
            //it was from watchdog. Clear the indication flag so it gets set again in WD ISR
            gv_wdInterrupt = 0;    //WARNING: This flag is cleared only here....

            //periph could be woken up if just 1 tick to WD wake

        }



        //TODO investigate whether is better to use that...
        //       UCSR0B |= bit (RXEN0);  // enable receiver
        //       UCSR0B |= bit (TXEN0);  // enable transmitter

        Serial.begin(9600);
        digitalWrite(LED_BUILTIN, 1);
    
    }
    else
    {
        digitalWrite(LED_BUILTIN, 1);
        //we do not go to sleep. The tick will return some time passed eventually
    }
  

}


