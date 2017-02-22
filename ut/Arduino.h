#ifndef FAKE_ARDUINO_H
#define FAKE_ARDUINO_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

using namespace std;

class Serial_if
{
public:
    virtual  uint8_t read() = 0;
    virtual  uint8_t peek() = 0;
    virtual  uint8_t write(const uint8_t * buff, size_t size ) = 0;
    virtual  uint8_t available() = 0;
};

class TSerial
{
public:
    uint8_t read() { return s->read(); };
    uint8_t peek() { return s->peek(); };
    uint8_t available() { return s->available(); };
    uint8_t write(const uint8_t * buff, size_t size)
    {
       return s->write(buff, size);
    }


    void install(Serial_if * ns)
    {
        s = ns;
    }

private:
    Serial_if * s;
} ;

extern TSerial Serial;



#endif