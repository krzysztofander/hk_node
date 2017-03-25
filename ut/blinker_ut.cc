#include "gtest/gtest.h" 
#include "gmock/gmock.h"
#include "Arduino.h"
#include "executor.h"
#include "comm.h"
#include "comm_defs.h"
#include "comm_common.h"
#include "serial.h"
#include "temp_measurement.h"
#include "mock.h"
#include "blinker.h"
#include "comm_extra_rec_handlers.h"
#include "comm_extra_records.h"

using ::testing::AtLeast;  
using ::testing::Return;
using ::testing::InSequence;
using ::testing::NiceMock;
using ::testing::StrictMock;
using ::testing::_;






//checking what happen when recieved unknown command...
TEST_F(SerialFixture, Blinker01)
{
    MockSupp mckSupp;
    MockTempMeasurement mckTM;
    NiceMock <MockSleeper > mckSLP;

    Blinker::setBlinkPattern(0x3);

    EXPECT_CALL(mckSupp, isButtonPressed()).WillRepeatedly(Return(false));
    EXPECT_CALL(mckSupp, blinkLed(0x3)).Times(1);
    Blinker::blinkAction();

    ASSERT_EQ(HKComm::g_commState.getState(), HKCommState::ESerialState::serialState_Action);
 
    EXPECT_CALL(mckTM, getSingleTempMeasurement()).WillOnce(Return(100));
    EXPECT_CALL(mckSLP, getUpTime()).WillRepeatedly(Return(200));
    
    HKComm::respondSerial();
    ASSERT_EQ(HKComm::g_commState.getState(), HKCommState::ESerialState::serialState_Respond);

    ASSERT_EQ(HKCommExtraRecordsHDL::dataReciever, &HKCommExtraHLRs::RTHdataReciever);
    ASSERT_EQ(HKCommExtraRecordsHDL::totalRecords, 0);

    const int VHTlenght = 3 + 1 + 1 + 1 + 3 + 1;

    EXPECT_CALL(mockSerial, write(_,VHTlenght ))
        .Times(1). WillRepeatedly(Return(VHTlenght));
    EXPECT_CALL(mockSerial, write(_, NUM_ELS(HKCommDefs::commandEOLOnResponceSequence)))
        .Times(1). WillRepeatedly(Return(NUM_ELS(HKCommDefs::commandEOLOnResponceSequence)));
    HKComm::respondSerial();
    ASSERT_EQ(HKComm::g_commState.getState(), HKCommState::ESerialState::serialState_Preable);
    
}




