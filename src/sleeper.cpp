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
#include "executor.h"
#include "serial.h"

//---------------------------------------------------------------
Sleeper::SleepTime Sleeper::g_sleepTime     = 0;
uint8_t            Sleeper::scale           = 6;
volatile Sleeper::UpTime  Sleeper::g_upTime          = 0;
//---------------------------------------------------------------

void Sleeper::setNextSleep(Sleeper::SleepTime  st)
{
    g_sleepTime = st;
}
Sleeper::SleepTime Sleeper::howMuchDidWeSleep(void)
{
    return 1; //todo Fix THAT
}
//http://www.gammon.com.au/forum/?id=11497

// watchdog interrupt
ISR (WDT_vect) 
{
    PCICR  &= ~bit (PCIE2); // disable pin change interrupts for D0 to D7
    Sleeper::incUpTime();
    Serial.begin(9600);
}  // end of WDT_vect

//pin interrupt. Will get triggered from serial
volatile uint8_t g_gotSerial = 0;

ISR (PCINT2_vect)
{
   PCICR  &= ~bit (PCIE2); // disable pin change interrupts for D0 to D7
   toggleBlue();
   g_gotSerial++;
   Serial.begin(9600);
   
}

void Sleeper::incUpTime(void)
{
    Sleeper::g_upTime++;
}

Sleeper::UpTime Sleeper::getUpTime(void)
{
    volatile  Sleeper::UpTime tempTime1;
    volatile  Sleeper::UpTime tempTime2;
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
          WDTCSR = bit (WDIE) | bit (WDP3) | bit (WDP0);    // set WDIE, and 8 seconds delay
       // WDTCSR = bit (WDIE) | bit (WDP3); 
        //WDTCSR = bit (WDIE) | bit (WDP2) | bit (WDP1) ;    // set WDIE, and 1 seconds delay
    
        g_upTime = 0U;
    
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


void Sleeper::gotToSleep(void)
{
        HKComm::echoLetter('E');
        HKComm::echoLetter(0);
        HKComm::echoLetter(PCMSK2);
        HKComm::echoLetter(PCIFR);
        HKComm::echoLetter(PCICR);
        HKComm::echoLetter(g_gotSerial);

    UpTime time = getUpTime();
    alert(uint8_t(time & 0xF), false);

    if (1 &&  ((g_gotSerial != 0 )&& g_sleepTime > 0))
    {
  //          EIMSK = 0;  //disable Int0, int 1
                                   //todo - move that to serial init...
            // pin change interrupt (example for D0)
  //          PCMSK2 = bit (PCINT16) | bit(PCINT19); // want pin 0 ONLY
            //PCMSK2 = bit (PCINT19); // want pin 3 ONLY
            //PCMSK2 = bit (PCINT16); // want pin 0 ONLY
  //          PCIFR  &= ~bit (PCIF2);   // clear any outstanding interrupts
  //          PCICR  = bit (PCIE2);   // enable pin change interrupts for D0 to D7
        for (int i = 0; i < 100; i++)
        {
          delay (16);
          digitalWrite(LED_BUILTIN, 1);
          delay (30);
          digitalWrite(LED_BUILTIN, 0);
        }
    }
    
    if (1 && g_sleepTime > 0 && g_gotSerial == 0 /*todo, day disable untill serial is active, for e.g. 3 seconds*/)
    {
        digitalWrite(LED_BUILTIN, 0);
        //finish off serial
        Serial.flush();
        Serial.end();
        digitalWrite(LED_BUILTIN, 1);
         // disable ADC
        ADCSRA = 0;  
        // clear various "reset" flags
        MCUSR = 0;     
 
        //set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
        set_sleep_mode (SLEEP_MODE_STANDBY);  
        //set_sleep_mode(SLEEP_MODE_IDLE);
        digitalWrite(LED_BUILTIN, 0);


        
      //  noInterrupts ();           // timed sequence follows
   //         UCSR0B &= ~bit (RXEN0);  // disable receiver
  //          UCSR0B &= ~bit (TXEN0);  // disable transmitter
            //attachInterrupt(digitalPinToInterrupt(0), ISR2_Recieved, LOW);
            //attachInterrupt(digitalPinToInterrupt(3), ISR2_Recieved, LOW);

            EIMSK = 0;  //disable Int0, int 1
            PCMSK2 = bit (PCINT16) | bit(PCINT19); // want pin 0 and 3 ONLY
            PCIFR  &= ~bit (PCIF2);   // clear any outstanding interrupts
            PCICR  |= bit (PCIE2);   // enable pin change interrupts for D0 to D7


            sleep_enable();

  
            // turn off brown-out enable in software
            //MCUCR = bit (BODS) | bit (BODSE);
            //MCUCR = bit (BODS); 
      //  interrupts (); 
    
      
        sleep_cpu ();  
        //SLEEPING HERE

         //GOT SOME INT
        // cancel sleep as a precaution
    
        sleep_disable();
 //       PCICR  &= ~bit (PCIE2); // disable pin change interrupts for D0 to D7
 //       UCSR0B |= bit (RXEN0);  // enable receiver
 //       UCSR0B |= bit (TXEN0);  // enable transmitter



        digitalWrite(LED_BUILTIN, 1);
    }
  
    // if (g_sleepTime > 0)
    if (0)
    {
        // Choose our preferred sleep mode:
        set_sleep_mode(SLEEP_MODE_IDLE);
     
        // Set sleep enable (SE) bit:
        sleep_enable();
        digitalWrite(LED_BUILTIN, 0);
        // Put the device to sleep:
        sleep_mode();
     
        // Upon waking up, sketch continues from this point.
        digitalWrite(LED_BUILTIN, 1);
        sleep_disable();

        delay(80);

    //delay(100 * g_sleepTime);
    }
}


