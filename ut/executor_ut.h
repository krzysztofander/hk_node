#ifndef EXECUTOR_UT_H
#define EXECUTOR_UT_H
#include "gtest/gtest.h" 
#include "gmock/gmock.h"
#include "Arduino.h"
#include "hk_time.h"

class MockSleeper
{
public:
    MOCK_METHOD0( getUpTime, HKTime::UpTime(void));

    static MockSleeper & instance()
    {
        static MockSleeper i;
        return i;
    }

};

class MockTempSensor
{
public:
    MOCK_METHOD0( readTemperature, float(void));

    static MockTempSensor & instance()
    {
        static MockTempSensor i;
        return i;
    }

};

class LooseFn_if
{
public:
    virtual void alert (uint8_t c, bool b) = 0;
    virtual void fake1(void) = 0;
    virtual void fake2(void) = 0;
    virtual void fake3(void) = 0;
};


class MockLooseFn : public LooseFn_if
{
public:
    MOCK_METHOD2( alert, void(uint8_t, bool b));
    MOCK_METHOD0( fake1, void(void));
    MOCK_METHOD0( fake2, void(void));
    MOCK_METHOD0( fake3, void(void));


    static MockLooseFn & instance()
    {
        static MockLooseFn mlf;
        return mlf;
    }

};

#endif