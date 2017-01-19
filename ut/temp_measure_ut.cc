#include "gtest/gtest.h" 
#include "gmock/gmock.h"
#include "Arduino.h"
#include "executor.h"
#include "temp_measurement.h"

using ::testing::AtLeast;  
using ::testing::Return;
using ::testing::InSequence;
using ::testing::NiceMock;
using ::testing::StrictMock;
using ::testing::_;


class MockSleeper
{
public:
    MOCK_METHOD0( getUpTime, HKTime::UpTime(void));

    static MockSleeper & instance()
    {
        static MockSleeper mlf;
        return mlf;
    }

};

HKTime::UpTime Sleeper::getUpTime()
{
    MockSleeper & i = MockSleeper::instance();
    return i.getUpTime();
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
    MockSleeper & s = MockSleeper::instance();
    ASSERT_EQ(TempMeasure::g_lastTempMeasurementIt, NUM_ELS(TempMeasure::g_tempMeasurements) - 1);

    EXPECT_CALL(s, getUpTime()).WillOnce(Return(2));
    TempMeasure::measureTemperature();
    ASSERT_EQ(TempMeasure::g_lastTempMeasurementIt, 0);
    ASSERT_EQ(TempMeasure::g_tempMeasurements[TempMeasure::g_lastTempMeasurementIt].timePassed, 2);
    
    EXPECT_CALL(s, getUpTime()).WillOnce(Return(22));
    TempMeasure::measureTemperature();
    ASSERT_EQ(TempMeasure::g_lastTempMeasurementIt, 1);
    ASSERT_EQ(TempMeasure::g_tempMeasurements[TempMeasure::g_lastTempMeasurementIt].timePassed, 20);

}

