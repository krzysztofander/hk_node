#include "gtest/gtest.h" 
#include "gmock/gmock.h"

#include "Arduino.h"
#include "executor.h"
#include "sleeper.h"
#include "temp_measurement.h"
#include "mock.h"

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



