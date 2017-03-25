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
#ifndef HK_COMM_COMMON_H
#define HK_COMM_COMMON_H

#include "hk_node.h"
#include "comm_defs.h"

class HKCommCommon
{
public:
     /*
    static uint8_t charToUnsigned(uint8_t givenChar, uint8_t *valToSet);

    static uint8_t dataToUnsignedShort(uint16_t offset, 
                                       const uint8_t (&inData)[HKCommDefs::commandMaxDataSize ],
                                       uint16_t & retVal);
    static uint8_t dataToUnsigned64(uint16_t offset,
                                           const uint8_t (&inData)[HKCommDefs::commandMaxDataSize],
                                           uint64_t & retVal);

    static uint8_t dataToUnsigned32(uint16_t offset,
                                    const uint8_t (&inData)[HKCommDefs::commandMaxDataSize],
                                    uint32_t & retVal);

    static uint8_t dataToUnsigned8(uint16_t offset,
                                   const uint8_t (&inData)[HKCommDefs::commandMaxDataSize],
                                   uint8_t & retVal);


    static uint8_t shortToData(uint16_t & inOutOffset, 
                               uint8_t (&inOutData)[HKCommDefs::commandMaxDataSize],
                               const uint16_t  inVal);

    static uint8_t uint8ToData(uint16_t & inOutOffset,
                                             uint8_t (&inOutData)[HKCommDefs::commandMaxDataSize],
                                             const uint8_t  inVal);

    static uint8_t uint32ToData(uint16_t & inOutOffset,
                               uint8_t (&inOutData)[HKCommDefs::commandMaxDataSize],
                               const uint32_t  inVal);
    static uint8_t uint64ToData(uint16_t & inOutOffset,
                               uint8_t (&inOutData)[HKCommDefs::commandMaxDataSize],
                               const uint64_t  inVal);
  
    static uint8_t formatMeasurement(uint16_t & inOutOffset, 
                                     uint8_t (&inOutData)[HKCommDefs::commandMaxDataSize],
                                     HKTime::SmallUpTime timeStamp, int16_t val);
    //@Brief parses the write the type as ascii
    //@Returns 0 if ok, serialErr_Assert  if error would go out of band
    template <class C>
    static uint8_t typeToData(uint16_t & inOutOffset,
                       uint8_t (&inOutData)[HKCommDefs::commandMaxDataSize],
                       const C inVal )
    {
        if (inOutOffset + sizeof(inVal) * 2 < sizeof(inOutData)  )
        {
            for (uint8_t i = 0; i < sizeof(inVal); i++)
            {
                uint8_t l = uint8_t(inVal >> (sizeof(inVal) - 1 - i) * 8);
                uint8_t highHex = l >> 4 ;
                highHex += highHex > uint8_t(9) ? uint8_t('a') - uint8_t(10) : uint8_t('0');
                uint8_t lowHex = l & uint8_t(0xF);
                lowHex += lowHex > uint8_t(9) ? uint8_t('a') - uint8_t(10) : uint8_t('0');

                inOutData[inOutOffset++] = highHex;
                inOutData[inOutOffset++] = lowHex;
            }
            return HKCommDefs::serialErr_None;
        }
        else
        {
            return HKCommDefs::serialErr_Assert;
        }

    }

    */

};
#endif
