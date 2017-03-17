#include "gtest/gtest.h" 
#include "gmock/gmock.h"

#include "Arduino.h"
#include "executor.h"
#include "sleeper.h"
#include "temp_measurement.h"
#include "mock.h"
#include "supp.h"

using ::testing::AtLeast;  
using ::testing::Return;
using ::testing::InSequence;
using ::testing::NiceMock;
using ::testing::StrictMock;
using ::testing::_;


TSerial Serial;



void alert (unsigned char c, bool b)
{

}
//------------------------------------------------------------------------
HKTime::UpTime Sleeper::getUpTime()
{
    MockSleeper & i = MockSleeper::instance();
    return i.getUpTime();
}
void Sleeper::setTime(const volatile HKTime::UpTime newTime)
{
    //TODO:
    //MockSleeper & i = MockSleeper::instance();
    //return i.setTime(newTime);
}


MockSleeper* MockSleeper::pInst = 0;
MockTempMeasurement* MockTempMeasurement::pInst = 0;
MockExecutor* MockExecutor::pInst = 0;
MockSupp * MockSupp::pInst = 0;

//------------------------------------------------------------------------
TempMeasure::TempMeasurement TempMeasure::getSingleTempMeasurement(void)
{
    return MockTempMeasurement::instance().getSingleTempMeasurement();
}
TempMeasure::TempRecord TempMeasure::getTempMeasurementRecord(uint16_t howManyRecordsBack)
{
    return MockTempMeasurement::instance().getTempMeasurementRecord(howManyRecordsBack);
}
//------------------------------------------------------------------------

void Executor::setExecutionTime(uint8_t t, Sleeper::SleepTime  s)
{
    MockExecutor::instance().setExecutionTime(t, s);
}

Sleeper::SleepTime Executor::giveExecutionTime(uint8_t t)
{
    return 0; //nothing in mock for now.
    //return MockExecutor::instance().giveExecutionTime(t);
}
//------------------------------
void Sleeper::setNoPowerDownPeriod(uint8_t noPowerDownTicks)
{
    ;
}

uint8_t Sleeper::getNoPowerDownPeriod()
{
    return 0;
}



void Sleeper::setPowerSaveMode(Sleeper::PowerSaveMode powerSaveMode)
{
    
}

uint8_t Sleeper::getPowerSaveMode()
{
    return 0;
}


bool Supp::isButtonPressed()
{
    return MockSupp::instance().isButtonPressed();
}
void Supp::blinkLed(uint8_t pattern)
{
    MockSupp::instance().blinkLed(pattern);
}
