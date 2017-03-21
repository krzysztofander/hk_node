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



#include "comm_common.h"


//@Brief parses the ASCII and fills the pointer with value
//@Returns 0 if ok, 
uint8_t HKCommCommon::charToUnsigned(uint8_t givenChar, uint8_t *valToSet)
{
    if (givenChar >= uint8_t('0') && givenChar <= uint8_t('9') )
    {
        *valToSet = givenChar - uint8_t('0');
        return 0;
    }
    else if (givenChar >= uint8_t('a') && givenChar <= uint8_t('f'))
    {
        givenChar -= (uint8_t('a') - uint8_t(10));
        *valToSet = givenChar;
        return uint8_t(0);
    }
    return uint8_t(1);
}



//@Brief parses the ASCII and fills the reference with value
//@Returns 0 if ok, serialErr_IncorrectNumberFormat  if error
template <class C>
uint8_t dataToType(uint16_t offset, const uint8_t (&inData)[HKCommDefs::commandMaxDataSize], C & retVal )
{
#if 1
    if (offset <= HKCommDefs::commandMaxDataSize - sizeof(C) * 2 - HKCommDefs::commandEOLSizeOnRecieve)
    {
        for (uint8_t i = 0; i < sizeof(C) * 2; i++)
        {
            uint8_t v;
            uint8_t r = HKCommCommon::charToUnsigned(inData[offset + i], &v);
            if (!!r)
            {
                return HKCommDefs::serialErr_Number_IncorrectFormat;
            }
            retVal <<= 4; //bits per digit
            retVal |= (C)v;
        }
        return 0;
    }
    else
    {
        return HKCommDefs::serialErr_Assert;
    }
#else
#endif
}


//@Brief parses the ASCII and fills the reference with value
//@Returns 0 if ok, serialErr_IncorrectNumberFormat  if error
uint8_t HKCommCommon::dataToUnsignedShort(uint16_t offset, 
                                          const uint8_t (&inData)[HKCommDefs::commandMaxDataSize], 
                                          uint16_t & retVal )
{
    return dataToType(offset, inData, retVal);
}

uint8_t HKCommCommon::dataToUnsigned64(uint16_t offset,
                                       const uint8_t (&inData)[HKCommDefs::commandMaxDataSize], 
                                       uint64_t & retVal )
{
    return dataToType(offset, inData, retVal);
}

uint8_t HKCommCommon::dataToUnsigned32(uint16_t offset,
                                       const uint8_t (&inData)[HKCommDefs::commandMaxDataSize], 
                                       uint32_t & retVal )
{
    return dataToType(offset, inData, retVal);
}

uint8_t HKCommCommon::dataToUnsigned8(uint16_t offset,
                                       const uint8_t (&inData)[HKCommDefs::commandMaxDataSize], 
                                       uint8_t & retVal )
{
    return dataToType(offset, inData, retVal);
}





uint8_t HKCommCommon::shortToData(uint16_t & inOutOffset,
                                  uint8_t (&inOutData)[HKCommDefs::commandMaxDataSize],
                                  const uint16_t  inVal )
{
    return typeToData(inOutOffset, inOutData, inVal);
}


uint8_t HKCommCommon::uint8ToData(uint16_t & inOutOffset,
                                  uint8_t (&inOutData)[HKCommDefs::commandMaxDataSize],
                                  const uint8_t  inVal )
{
    return typeToData(inOutOffset, inOutData, inVal);
}

uint8_t HKCommCommon::uint32ToData(uint16_t & inOutOffset,
                                  uint8_t (&inOutData)[HKCommDefs::commandMaxDataSize],
                                  const uint32_t  inVal )
{
    return typeToData(inOutOffset, inOutData, inVal);
}

uint8_t HKCommCommon::uint64ToData(uint16_t & inOutOffset,
                                   uint8_t (&inOutData)[HKCommDefs::commandMaxDataSize],
                                   const uint64_t  inVal )
{
    return typeToData(inOutOffset, inOutData, inVal);
}



//@brief formats the measurement time/val pair directly into buffer
//@param [inout] inOutOffset:  base and new offset in buffer
//@param [inout] inoutData:  buffer where to input the function
//@param [in ] inoutDataSize:  size of buffer
//@return 0 if ok, serialErr_Assert  if error
uint8_t HKCommCommon::formatMeasurement(uint16_t & inOutOffset,
                                        uint8_t (&inOutData)[HKCommDefs::commandMaxDataSize],
                                        HKTime::SmallUpTime timeStamp, 
                                        int16_t val)
{
    static const char chStart = '(';
    static const char chSeparator = ',';
    static const char chEnd   = ')';

    if (inOutOffset
        + sizeof(chStart)
        + sizeof(uint16_t) * 2 *2 
        + sizeof(chEnd)
        + sizeof(uint16_t) * 2
        + sizeof(chEnd) < sizeof(inOutData))
    {
        inOutData[inOutOffset++]=chStart;
        shortToData(inOutOffset, inOutData, uint16_t(timeStamp >> 16)); //MSB first
        shortToData(inOutOffset, inOutData, uint16_t(timeStamp));       //LSB
        inOutData[inOutOffset++]=chSeparator;
        shortToData(inOutOffset, inOutData, val);
        inOutData[inOutOffset++]=chEnd;

#if 1
        for (uint8_t i = 0; i < NUM_ELS(HKCommDefs::commandEOLOnResponceSequence); i++)
        {
            inOutData[inOutOffset++] = HKCommDefs::commandEOLOnResponceSequence[i];
        }
#endif        
        return HKCommDefs::serialErr_None;
    }
    else
    {
        return HKCommDefs::serialErr_Assert;
    }
}



