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
#include <OneWire.h>
#include <DS18B20.h>
#include "temp_sensor.h"

class OneWireWrap
{
public:
    static const uint8_t  ONEWIRE_PIN = 4;
    static OneWire onewire; 

    static DS18B20 sensor;

};

OneWire  OneWireWrap::onewire(OneWireWrap::ONEWIRE_PIN);
DS18B20  OneWireWrap::sensor(&OneWireWrap::onewire);

uint8_t TempSensor::address[8];

uint8_t TempSensor::init()
{
        
    //sensor not found
    if (!(findSensor()))
    {
        return 0;
    }
    OneWireWrap::sensor.begin(sensorResolution);
    return 1;
}

//size of address array>=8
uint8_t TempSensor::findSensor()
{
    while(OneWireWrap::onewire.search(address))
    {
        //not temp sensor
        if (address[0] != 0x28)
        {
            continue;
        }   
        //Incorrect crc
        if (OneWire::crc8(address, 7) != address[7])
        {
            continue;
        }
        else //found sensor
        {
            return 1;
        }
    }
    return 0;
}


float TempSensor::readTemperature()
{
    OneWireWrap::sensor.request(address);
    while(!OneWireWrap::sensor.available())
    {
        //timeout is included in the sensor
    }
    return OneWireWrap::sensor.readTemperature(address);
}

