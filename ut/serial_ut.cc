#include "gtest/gtest.h" 
#include "Arduino.h"
#include "executor.h"

TSerial Serial;

class SerialImpl : public Serial_if
{
public:
    virtual  uint8_t  read()
    {
        return 'e';
    }
    virtual  uint8_t available()
    {
        return 0;
    }
    virtual  uint8_t write(const uint8_t * buff, size_t size) 
    {
        return (uint8_t)size;
    }


};

void alert (unsigned char c, bool b)
{

}


void Executor::setExecutionTime(uint8_t t, uint16_t s)
{

}


TEST(Serial, t1)
{
    SerialImpl myS;
    Serial.install(&myS);

    ASSERT_TRUE (1);
}

