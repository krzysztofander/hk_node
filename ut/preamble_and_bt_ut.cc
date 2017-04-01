#include "gtest/gtest.h" 
#include "gmock/gmock.h"
#include "Arduino.h"
#include "executor.h"
#include "comm.h"
#include "comm_defs.h"
#include "serial.h"
#include "temp_measurement.h"
#include "mock.h"

using ::testing::AtLeast;  
using ::testing::Return;
using ::testing::InSequence;
using ::testing::NiceMock;
using ::testing::StrictMock;
using ::testing::_;

TEST_F (PreableTestFixture, initPreample)
{
    EXPECT_CALL(mockSerial, write(_,_))
        .WillRepeatedly(Return(8));


    HKSerial::init();
   
    EXPECT_CALL(mockSerial, available())
        .WillRepeatedly(Return(0));


    HKSerial::activate();ASSERT_EQ(HKSerial::isActive(), false);
    HKSerial::activate();ASSERT_EQ(HKSerial::isActive(), false);
    HKSerial::activate();ASSERT_EQ(HKSerial::isActive(), false);
    HKSerial::activate();ASSERT_EQ(HKSerial::isActive(), false);
    HKSerial::activate();ASSERT_EQ(HKSerial::isActive(), false);
    HKSerial::activate();ASSERT_EQ(HKSerial::isActive(), false);
    HKSerial::activate();ASSERT_EQ(HKSerial::available(), 0);
    ASSERT_EQ(HKSerial::g_preableState,HKSerial::PreableState:: none);
}

TEST_F (PreableTestFixture, testSM)
{
    EXPECT_CALL(mockSerial, write(_,_))
        .WillRepeatedly(Return(8));
    
    HKSerial::init();

    EXPECT_CALL(mockSerial, available())
        .WillOnce(Return(0));
    HKSerial::activate();ASSERT_EQ(HKSerial::isActive(), false);


    EXPECT_CALL(mockSerial, available())
        .WillRepeatedly(Return(100));
    EXPECT_CALL(mockSerial, read())
        .WillOnce(Return('x'));
    HKSerial::activate();ASSERT_EQ(HKSerial::isActive(), true);

    EXPECT_CALL(mockSerial, available())
        .WillRepeatedly(Return(100));
    EXPECT_CALL(mockSerial, read())
        .WillOnce(Return('#'));
    ASSERT_EQ(HKSerial::available(), 0);

    ASSERT_EQ(HKSerial::g_preableState, HKSerial::PreableState::nonRecognizedChar);
    //Now lets exit preamble state

    EXPECT_CALL(mockSerial, available())
        .WillRepeatedly(Return(100));
    EXPECT_CALL(mockSerial, read())
        .WillOnce(Return('!'));
    ASSERT_EQ(HKSerial::available(), 0);
    ASSERT_EQ(HKSerial::g_preableState, HKSerial::PreableState::exclamation);


    EXPECT_CALL(mockSerial, available())
        .WillRepeatedly(Return(100));
    EXPECT_CALL(mockSerial, read())
        .WillOnce(Return('#'));

    ASSERT_EQ(HKSerial::available(), 100);
    ASSERT_EQ(HKSerial::g_preableState, HKSerial::PreableState::finished);
    //lets see if its locked

    HKSerial::activate();ASSERT_EQ(HKSerial::isActive(), true);
    HKSerial::activate();ASSERT_EQ(HKSerial::isActive(), true);
    HKSerial::activate();ASSERT_EQ(HKSerial::isActive(), true);
   
    EXPECT_CALL(mockSerial, available())
        .WillOnce(Return(100));
    ASSERT_EQ(HKSerial::available(), 100);

    
    //good, now lets get back to preable state

    HKSerial::commandProcessed();
    ASSERT_EQ(HKSerial::g_preableState, HKSerial::PreableState::none);

    EXPECT_CALL(mockSerial, available())
        .WillOnce(Return(0));
    ASSERT_EQ(HKSerial::available(), 0);

    EXPECT_CALL(mockSerial, available())
        .WillOnce(Return(0));

    HKSerial::activate();ASSERT_EQ(HKSerial::isActive(), false);

    //on disconnect
    EXPECT_CALL(mockSerial, available())
        .WillRepeatedly(Return(100));
    EXPECT_CALL(mockSerial, read())
        .WillOnce(Return('L'));
    HKSerial::activate();ASSERT_EQ(HKSerial::isActive(), true);


    EXPECT_CALL(mockSerial, available())
        .WillRepeatedly(Return(100));
    EXPECT_CALL(mockSerial, read())
        .WillOnce(Return('O'));
    HKSerial::activate();ASSERT_EQ(HKSerial::isActive(), true);
    

    EXPECT_CALL(mockSerial, available())
        .WillRepeatedly(Return(100));
    EXPECT_CALL(mockSerial, read())
        .WillOnce(Return('S'));
    HKSerial::activate();ASSERT_EQ(HKSerial::isActive(), true);

    //on disconnect
    EXPECT_CALL(mockSerial, available())
        .WillRepeatedly(Return(100));
    EXPECT_CALL(mockSerial, read())
        .WillOnce(Return('T'));
    EXPECT_CALL(mockSerial,write(_,8))
                .WillOnce(Return(8));
    HKSerial::activate();ASSERT_EQ(HKSerial::isActive(), false);


}

TEST_F (PreableTestFixture, testTimeout)
{
    EXPECT_CALL(mockSerial, write(_, _))
        .WillRepeatedly(Return(8));

    HKSerial::init();

    EXPECT_CALL(mockSerial, available())
        .WillOnce(Return(0));
    HKSerial::activate();ASSERT_EQ(HKSerial::isActive(), false);


    EXPECT_CALL(mockSerial, available())
        .WillRepeatedly(Return(100));
    EXPECT_CALL(mockSerial, read())
        .WillOnce(Return('x'));
    HKSerial::activate();ASSERT_EQ(HKSerial::isActive(), true);
    
    for (int i = 0; i < HKSerial::g_preableFinishTime - 1; i++)
    {
        EXPECT_CALL(mockSerial,available()).WillRepeatedly(Return(0));
        HKSerial::nextLoop(i);
        ASSERT_EQ(HKSerial::g_preableState, HKSerial::PreableState::nonRecognizedChar);
    }
    EXPECT_CALL(mockSerial,available()).WillRepeatedly(Return(0));
    HKSerial::nextLoop(HKSerial::g_preableFinishTime -1);
    
    ASSERT_EQ(HKSerial::g_preableState, HKSerial::PreableState::none);
  
    EXPECT_CALL(mockSerial,available()).WillRepeatedly(Return(0));
    HKSerial::activate();ASSERT_EQ(HKSerial::isActive(), false);


}