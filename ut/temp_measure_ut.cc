#include "gtest/gtest.h" 
#include "gmock/gmock.h"
#include "Arduino.h"
#include "executor.h"
#include "temp_measurement.h"
#include "executor_ut.h"
#include "temp_sensor.h"

using ::testing::AtLeast;  
using ::testing::Return;
using ::testing::InSequence;
using ::testing::NiceMock;
using ::testing::StrictMock;
using ::testing::_;



HKTime::UpTime Sleeper::getUpTime()
{
    MockSleeper & i = MockSleeper::instance();
    return i.getUpTime();
}

float TempSensor::readTemperature(void)
{
    MockTempSensor & i = MockTempSensor::instance();
    return i.readTemperature();
}


TEST(HKTimeTest, t1)
{


    HKTime::ShortTimeDiff d;

    d = HKTime::getShortDiff(1, 0);
    ASSERT_EQ(d, HKTime::ShortTimeDiff(1));

    d = HKTime::getShortDiff(0, 1);
    ASSERT_EQ(d, HKTime::ShortTimeDiff(-1));

    d = HKTime::getShortDiff(0xFFFFULL, 0xFFFEULL);
    ASSERT_EQ(d, HKTime::ShortTimeDiff(1));

    d = HKTime::getShortDiff(0xFFFEULL, 0xFFFFULL);
    ASSERT_EQ(d, HKTime::ShortTimeDiff(-1));

    d = HKTime::getShortDiff(0x10000ULL, 0xFFFFULL);
    ASSERT_EQ(d, HKTime::ShortTimeDiff(1));

    d = HKTime::getShortDiff(0xFFFFULL, 0x10000ULL);
    ASSERT_EQ(d, HKTime::ShortTimeDiff(-1));

    d = HKTime::getShortDiff(0x1000000000000000ULL, 0x0FFFFFFFFFFFFFFFULL);
    ASSERT_EQ(d, HKTime::ShortTimeDiff(1));

    d = HKTime::getShortDiff(0x10000ULL, 0x0ULL);
    ASSERT_EQ(d, HKTime::ShortTimeDiff(0x7FFF));

    d = HKTime::getShortDiff(0x11111ULL, 0x111ULL);
    ASSERT_EQ(d, HKTime::ShortTimeDiff(0x7FFF));

    d = HKTime::getShortDiff( 0x0ULL, 0x10000ULL);
    ASSERT_EQ(d, HKTime::ShortTimeDiff(-0x8000));

};

TEST(TempMeasurementTest, t1)
{
    MockSleeper    &   s = MockSleeper::instance();
    MockTempSensor & mTS = MockTempSensor::instance();

    TempMeasure::initMeasureTemperature();
    ASSERT_EQ(TempMeasure::g_lastTempMeasurementIt, NUM_ELS(TempMeasure::g_tempMeasurements) - 1);  //it is expected to be roll over on first call

    EXPECT_CALL(s, getUpTime()).WillOnce(Return(2));
    EXPECT_CALL(mTS, readTemperature()).WillOnce(Return(122.25f));

    TempMeasure::measureTemperature();
    ASSERT_EQ(TempMeasure::g_lastTempMeasurementIt, 0);
    ASSERT_EQ(TempMeasure::g_tempMeasurements[TempMeasure::g_lastTempMeasurementIt].timeStamp, 2);
    ASSERT_EQ(TempMeasure::g_tempMeasurements[TempMeasure::g_lastTempMeasurementIt].tempFPCelcjus, 122*16 + 4);

    EXPECT_CALL(s, getUpTime()).WillOnce(Return(22));
    EXPECT_CALL(mTS, readTemperature()).WillOnce(Return(-11.25f));
    TempMeasure::measureTemperature();
    ASSERT_EQ(TempMeasure::g_lastTempMeasurementIt, 1);
    ASSERT_EQ(TempMeasure::g_tempMeasurements[TempMeasure::g_lastTempMeasurementIt].timeStamp, 22);
    ASSERT_EQ(TempMeasure::g_tempMeasurements[TempMeasure::g_lastTempMeasurementIt].tempFPCelcjus, -11*16 - 4);

}

TEST(TempMeasurementTest, t2)
{
    MockSleeper    &   s = MockSleeper::instance();
    MockTempSensor & mTS = MockTempSensor::instance();

    TempMeasure::initMeasureTemperature();
    ASSERT_EQ(TempMeasure::g_lastTempMeasurementIt, NUM_ELS(TempMeasure::g_tempMeasurements) - 1);  //it is expected to be roll over on first call

    const int measurements = NUM_ELS(TempMeasure::g_tempMeasurements);
    

    for (int i = 0; i < measurements ; i++)
    {
        EXPECT_CALL(s, getUpTime()).WillOnce(Return(i));
        EXPECT_CALL(mTS, readTemperature()).WillOnce(Return( float (i * 1.0 / 16.0)) );
        TempMeasure::measureTemperature();
    }
    
    ASSERT_EQ(TempMeasure::g_lastTempMeasurementIt, NUM_ELS(TempMeasure::g_tempMeasurements) - 1);  //it is expected to be roll over on first call

    for (int i = 0; i < measurements; i++)
    {
        ASSERT_EQ(TempMeasure::g_tempMeasurements[i].timeStamp, i);
        ASSERT_EQ(TempMeasure::g_tempMeasurements[i].tempFPCelcjus, i );
    }
    //now, check the get function
    for (int i = 0; i < measurements; i++)
    {
        TempMeasure::TempRecord rec(measurements -1 - i, measurements -i - 1 );
        ASSERT_EQ(TempMeasure::getTempMeasurementRecord(i).timeStamp,rec.timeStamp );
        ASSERT_EQ(TempMeasure::getTempMeasurementRecord(i).tempFPCelcjus,rec.tempFPCelcjus);
    }
    // test over range
    TempMeasure::TempRecord rec(0,0);
    ASSERT_EQ(TempMeasure::getTempMeasurementRecord(1234).timeStamp,rec.timeStamp );
    ASSERT_EQ(TempMeasure::getTempMeasurementRecord(2345).tempFPCelcjus,rec.tempFPCelcjus);

}
