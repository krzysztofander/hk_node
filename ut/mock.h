#ifndef HK_MOCK_H
#define HK_MOCK_H
#include "gtest/gtest.h" 
#include "gmock/gmock.h"


#include "comm.h"
#include "comm_defs.h"

#include "Arduino.h"
#include "temp_measurement.h"

//using ::testing::AtLeast;  
//using ::testing::Return;
using ::testing::InSequence;
using ::testing::NiceMock;
using ::testing::StrictMock;

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

class MockSerial : public Serial_if
{
public:
    MOCK_METHOD0(read, uint8_t());
    MOCK_METHOD0(peek, uint8_t());
    MOCK_METHOD0(available, uint8_t());
    MOCK_METHOD2(write, uint8_t(const uint8_t*, size_t));

};

class MockLooseFn 
{
public:
    MOCK_METHOD2(alert, void(uint8_t, bool b));
    static MockLooseFn & instance()
    {
        static MockLooseFn  f;
        return f;
    }
};

class MockExecutor
{
public:
    MOCK_METHOD2(setExecutionTime, void(uint8_t, uint16_t));

    static MockExecutor & instance()
    {
        static MockExecutor  f;
        return f;
    }
};


class MockTempMeasurement
{
public:
    MOCK_METHOD0(getSingleTempMeasurement, uint16_t(void));
    MOCK_METHOD1(getTempMeasurementRecord, TempMeasure::TempRecord(uint16_t));

    static MockTempMeasurement & instance()
    {
        static MockTempMeasurement  f;
        return f;
    }

};


//------------------------------------------------------------------------

class SerialFixture : public ::testing::Test
{
public: 
    MockSerial mockSerial;
    InSequence dummy;

    virtual void SetUp()
    {
        Serial.install(&mockSerial);
        HKComm::g_SerialState =  HKCommDefs::serialState_ReadCommand;
        HKComm::g_dataIt = 0;
        HKComm::g_serialError = 0;
    }
};

class NiceSerialFixture : public ::testing::Test
{
public: 
    NiceMock<MockSerial>  mockSerial;
    InSequence dummy;

    virtual void SetUp()
    {
        Serial.install(&mockSerial);
        HKComm::g_SerialState =  HKCommDefs::serialState_ReadCommand;
        HKComm::g_dataIt = 0;
        HKComm::g_serialError = 0;

    }

};

class StrictSerialFixture : public ::testing::Test
{
public: 
    StrictMock<MockSerial>  mockSerial;
    InSequence dummy;

    virtual void SetUp()
    {
        Serial.install(&mockSerial);

        HKComm::g_SerialState =  HKCommDefs::serialState_ReadCommand;
        HKComm::g_dataIt = 0;
        HKComm::g_serialError = 0;
    }

};

class Serial_D_method_Fixture : public SerialFixture
{
public:
    uint16_t dataSize;

};

class Serial_R_method_Fixture : public SerialFixture
{
public:
    uint16_t dataSize;

};
#endif

