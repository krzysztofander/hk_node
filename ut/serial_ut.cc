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


TEST_F(NiceSerialFixture, notEnoutDataAvailable)
{
   // SetUp();
    int8_t retVal = 0;
    //check if function exits when not enouth data available
    //state is read command
    EXPECT_CALL(mockSerial, available())
        .WillOnce(Return(2))
        .WillRepeatedly(Return(0));
    EXPECT_CALL(mockSerial, read())
        .WillRepeatedly(Return('D'));
    retVal = HKComm::respondSerial();

    EXPECT_CALL(mockSerial, available())
        .WillOnce(Return(0));
    retVal += HKComm::respondSerial();

    EXPECT_CALL(mockSerial, available())
        .WillOnce(Return(0));
    retVal += HKComm::respondSerial();

    ASSERT_EQ(retVal, 0); //returned 0, no need to change state
    ASSERT_EQ(HKComm::g_commState.getState(), HKCommState::ESerialState::serialState_ParseCommand);

}


TEST_F(NiceSerialFixture, changeStateError_serialErr_eolInCommand01)
{
    int8_t retVal = 0;
 
    EXPECT_CALL(mockSerial, available())
        .WillRepeatedly(Return(10));

    EXPECT_CALL(mockSerial, read())
        .WillOnce(Return('D'))
        .WillOnce(Return('D'))
        .WillOnce(Return(HKCommDefs::commandEOLSignOnRecieve));
    
    retVal = HKComm::respondSerial();
    ASSERT_EQ(retVal, 0);
    retVal = HKComm::respondSerial();
    ASSERT_EQ(retVal, 0);
    retVal = HKComm::respondSerial();
    ASSERT_EQ(retVal, 1);

    ASSERT_EQ(HKComm::g_commState.getState(), HKCommState::ESerialState::serialState_Error);
    ASSERT_EQ(HKComm::g_commState.getErrorType(), HKCommState::ESerialErrorType::serialErr_Parser);

}

//checking what happen when recieved unknown command...
TEST_F(NiceSerialFixture, changeStateError_serialErr_unknownCommand)
{
    int8_t retVal = 0;

    EXPECT_CALL(mockSerial, available())
        .WillRepeatedly(Return(1));
    EXPECT_CALL(mockSerial, read())
        .WillOnce(Return('D'))
        .WillOnce(Return('D'))
        .WillOnce(Return('D')) //Command is correct, but unknown
        .WillOnce(Return(HKCommDefs::commandEOLSignOnRecieve));

   
    retVal = HKComm::respondSerial();
    retVal = HKComm::respondSerial();
    retVal = HKComm::respondSerial();
    retVal = HKComm::respondSerial();
    ASSERT_EQ(HKComm::g_commState.getState(), HKCommState::ESerialState::serialState_Action);

 
    retVal = HKComm::respondSerial();

    ASSERT_EQ(HKComm::g_commState.getState(), HKCommState::ESerialState::serialState_Error);
    ASSERT_EQ(HKComm::g_commState.getErrorType(), HKCommState::ESerialErrorType::serialErr_LogicNoSuchCommand);

 
}

TEST_F(NiceSerialFixture, changeStateError_serialErr_unknownCommandNoEolInData)
{
   //todo

}


TEST_F (Serial_D_method_Fixture, echoD)
{
    bool retVal;
    EXPECT_CALL(mockSerial, available())
        .WillRepeatedly(Return(1));
    EXPECT_CALL(mockSerial, read())
        .WillOnce(Return('D'))
        .WillOnce(Return('E'))
        .WillOnce(Return('R')) //Command is correct, but unknown
        .WillOnce(Return(HKCommDefs::commandEOLSignOnRecieve));


    retVal = HKComm::respondSerial();
    retVal = HKComm::respondSerial();
    retVal = HKComm::respondSerial();
    retVal = HKComm::respondSerial();
    ASSERT_EQ(HKComm::g_commState.getState(), HKCommState::ESerialState::serialState_Action);
    retVal = HKComm::respondSerial();
    ASSERT_EQ(HKComm::g_commState.getState(), HKCommState::ESerialState::serialState_Respond);
    EXPECT_CALL(mockSerial, write(_, 3))
        .Times(1). WillRepeatedly(Return(3));
    EXPECT_CALL(mockSerial, write(_, (size_t)NUM_ELS(HKCommDefs::commandEOLOnResponceSequence)))
        .Times(1). WillRepeatedly(Return((uint8_t)NUM_ELS(HKCommDefs::commandEOLOnResponceSequence)));
    retVal = HKComm::respondSerial();
    ASSERT_EQ(HKComm::g_commState.getState(), HKCommState::ESerialState::serialState_Preable);


}


TEST_F (Serial_D_method_Fixture, checkWriteError)
{
    bool retVal;
    EXPECT_CALL(mockSerial, available())
        .WillRepeatedly(Return(1));
    EXPECT_CALL(mockSerial, read())
        .WillOnce(Return('D'))
        .WillOnce(Return('E'))
        .WillOnce(Return('R')) //Command is correct, but unknown
        .WillOnce(Return(HKCommDefs::commandEOLSignOnRecieve));


    retVal = HKComm::respondSerial();
    retVal = HKComm::respondSerial();
    retVal = HKComm::respondSerial();
    retVal = HKComm::respondSerial();
    ASSERT_EQ(HKComm::g_commState.getState(), HKCommState::ESerialState::serialState_Action);
    retVal = HKComm::respondSerial();
    ASSERT_EQ(HKComm::g_commState.getState(), HKCommState::ESerialState::serialState_Respond);
    
    
    EXPECT_CALL(mockSerial, write(_, 3))
        .Times(1). WillRepeatedly(Return(3));
    EXPECT_CALL(mockSerial, write(_, NUM_ELS(HKCommDefs::commandEOLOnResponceSequence)))
        .Times(1). WillRepeatedly(Return(0 /*error here*/));
    retVal = HKComm::respondSerial();
    ASSERT_EQ(HKComm::g_commState.getState(), HKCommState::ESerialState::serialState_Error);

    retVal = HKComm::respondSerial();
    ASSERT_EQ(HKComm::g_commState.getState(), HKCommState::ESerialState::serialState_Respond);

    const int sizeofErrorResp = 0x18;

    EXPECT_CALL(mockSerial, write(_, sizeofErrorResp))
        .Times(1). WillRepeatedly(Return(sizeofErrorResp));
    EXPECT_CALL(mockSerial, write(_, NUM_ELS(HKCommDefs::commandEOLOnResponceSequence)))
        .Times(1). WillRepeatedly(Return((uint8_t)NUM_ELS(HKCommDefs::commandEOLOnResponceSequence)));

    retVal = HKComm::respondSerial();


    ASSERT_EQ(HKComm::g_commState.getState(), HKCommState::ESerialState::serialState_Preable);

}


