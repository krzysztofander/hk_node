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

void setupBody() 
{
    // put your setup code here, to run once:
    // initialize digital pin LED_BUILTIN as an output.
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    initAllFunctions();
    setupDoWhatYouShouldTab();
    Serial.println("hello world");
    for (int i = 0; i < 10; i++)
    {
        digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(20);                       // wait for a second
        digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
        delay(20);                       // wait for a second
    }
}

void loopBody() 
{
    //just woke up

    //check reason
    WakeUpReason wkReason = getWKReason();
    //do actions
    if (wkReason == WakeUpReason_timeout)
    {
        setNextSleep(getDefaultSleepTime());
        doWhatYouShould();
    }
    else if (wkReason == WakeUpReason_serial)
    {
        setNextSleep(getRemainingSleepTime());
        respondSerial();
    }
    else
    {
        alert(AlertReason_unknownWakeUp);
        Serial.println("unknowns wakeup: ");
        Serial.println(wkReason, HEX );
    }

    //go to sleep

}
//------------------------------------------------------------------

WakeUpReason getWKReason(void)
{
    //todo - fix that
    return WakeUpReason_serial;
}

void alert(register AlertReason reason)
{
    if (AlertReason_unknownWakeUp == reason )
    {  
        int pwm =10;
        for ( pwm = 10; pwm <= 90; pwm +=5)
        {
            digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
            delay(100-pwm);                       // wait for a second
            digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
            delay(pwm);                       // wait for a second
        }

        for (; pwm >= 10; pwm -=5)
        {
            digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
            delay(100-pwm);                       // wait for a second
            digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
            delay(pwm);                       // wait for a second
        }
    }
    else if (AlertReason_serialChar == reason)
    {  
        for (int i = 0; i < 5; i++)
        {
            digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
            delay(50);                       // wait for a second
            digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
            delay(50);                       // wait for a second
        }
    }
    else if (AlertReason_serialSend == reason)
    {  
        for (int i = 0; i < 2; i++)
        {
            digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
            delay(400);                       // wait for a second
            digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
            delay(400);                       // wait for a second
        }
    }
    else 
    {
        digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(1000);                       // wait for a second
        digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
        delay(1000);                       // wait for a second
    }
}

//------------------------------------------------------------------
DoWhatYouShould g_doWhatYouShouldTab[MaxDoWhatYouShould];

void initAllFunctions(void)
{
    initMeasureTemperature();
}
void setupDoWhatYouShouldTab(void)
{
    for (unsigned char i = 0; i < NUM_ELS(g_doWhatYouShouldTab) ; i++)
    {
        g_doWhatYouShouldTab[i] = 0;
    }
    g_doWhatYouShouldTab[0] = measureTemperature;
}

void doWhatYouShould(void)
{
    unsigned char i = 0;
    while (g_doWhatYouShouldTab[i] != 0 && i < NUM_ELS(g_doWhatYouShouldTab)  )
    {
        g_doWhatYouShouldTab[i]();
    }
}

SleepTime getRemainingSleepTime(void)
{
    return 0;
}

SleepTime getDefaultSleepTime(void)
{
    return 0;
}

void setNextSleep(SleepTime  st)
{

}

