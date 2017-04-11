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
#include "nv.h"
#include <EEPROM.h>
#include "adc.h"
//------------------------------------------------------------------


void ADCSupport::adcPrepare(ADCSupport::ADCSupportConfigRefSrc ref, uint8_t src)
{
 
    ADCSRA |= (1 << ADEN);          //enable ADC
    ADCSRA &= ~(1 << ADIE);         //we do not want interrupt

    uint8_t newAmux = 0;

    newAmux |= (0 << ADLAR);

    if (ref == ADCSupport::ADCSupportConfigRefSrc::referenceInternal)
    {
        newAmux |= (1 << REFS1) | (1 << REFS0);
    }
    else
    {
        newAmux |= (0 << REFS1) | (1 << REFS0);//AVCC with external capacitor at AREF pin
    }
    
    newAmux |=  ((src & 0xF) << MUX0);

    ADMUX = newAmux;

}

int16_t ADCSupport::adcRun(int8_t loops)
{
    int16_t result;
    for (int8_t i = 0; i < loops; i++)
    {

        ADCSRA |= (1 << ADSC);
        // Wait for it to complete
        while (((ADCSRA & (1 << ADSC)) != 0));
        result = ADC;

    }
    return result;
}
void ADCSupport::adcClose()
{
    ADCSRA &= ~(1 << ADEN);          //Disable ADC

}

int16_t ADCSupport::selectInternalReference()
{

    int16_t results;

    adcPrepare(ADCSupport::ADCSupportConfigRefSrc::referenceInternal,
               static_cast<uint8_t> (14)); //1110 1.1V (VBG)
    
    //we should have both reference and measurement of the VBG

    results = adcRun(static_cast<int8_t>(
        ADCSupport::ADCSupportConfig::bateryMeasurementsSeries
    ));
    adcClose();
    return results;
}

int16_t ADCSupport::readBandgap()
{
#if 1
    int16_t results;
    int16_t internalReferenceVoltage;
    NV::read(NV::NVData::nvBandgapVoltage, &internalReferenceVoltage);

    adcPrepare(ADCSupport::ADCSupportConfigRefSrc::referenceACC,
               static_cast<uint8_t> (14)); //1110 1.1V (VBG)
    results = adcRun(static_cast<int8_t>(
                        ADCSupport::ADCSupportConfig::bateryMeasurementsSeries
                                        ) );
    adcClose();
    
    // Scale the value
    results = static_cast<int16_t>(
        (static_cast<int32_t>(internalReferenceVoltage) << 10) / results
        );


    return results;

#else
    int16_t internalReferenceVoltage;
    NV::read(NV::NVData::nvBandgapVoltage, &internalReferenceVoltage);

    int16_t results;


    // REFS1 REFS0          --> 0 1, AVcc internal ref.
    // MUX3 MUX2 MUX1 MUX0  --> 1110 1.1V (VBG)


    ADCSRA |= (1 << ADEN);          //enable ADC
    ADCSRA &= ~(1 << ADIE);         //we do not want interrupt

    ADMUX = (0 << REFS1) | (1 << REFS0)   //AVCC with external capacitor at AREF pin
        | (0 << ADLAR)
        | (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (0 << MUX0);
    // Start a conversion  

    for (int8_t i = 0; i < static_cast<int8_t>(ADCSupportConfig::bandgapMeasurementsSeries); i++)
    {

        ADCSRA |= (1 << ADSC);
        // Wait for it to complete
        while (((ADCSRA & (1 << ADSC)) != 0));
        //results = ADC;
        // Scale the value
        // results = (((InternalReferenceVoltage * 1024L) / ADC) + 5L) / 10L;
        results = static_cast<int16_t>(
            (static_cast<int32_t>(internalReferenceVoltage) << 10) / ADC
                                       );

    }

    ADCSRA &= ~(1 << ADEN);          //Disable ADC


    return results;
#endif
}




//---------------------------------------------------------------
