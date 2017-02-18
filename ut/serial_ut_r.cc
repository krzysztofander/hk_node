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

using ::testing::AtLeast;  
using ::testing::Return;
using ::testing::InSequence;
using ::testing::NiceMock;
using ::testing::StrictMock;
using ::testing::_;

#if 1

TEST_F (Serial_R_method_Fixture, RTM)
{
    MockTempMeasurement  tm;
    MockSleeper s;


    uint8_t inOutCommand[HKCommDefs::commandSize] = 
    {   //command to send
        'R',
        'T',
        'M' 
    };
    uint8_t inOutData[HKCommDefs::commandMaxDataSize];
    std::fill_n(inOutData, 0,HKCommDefs:: commandMaxDataSize);
    inOutData[0] = HKCommDefs::commandEOLSignOnRecieve;

    EXPECT_CALL(tm, getSingleTempMeasurement()).Times(1).WillOnce(Return(0x12ac));
    EXPECT_CALL(s, getUpTime()).WillOnce(Return(0xabcd12345678ll));

    uint8_t retVal = HKComm::command_R(inOutCommand,inOutData,dataSize);

    //expected results:
    ASSERT_EQ(inOutCommand[0],'V');  //what to expect in command
    ASSERT_EQ(inOutCommand[1],'T');
    ASSERT_EQ(inOutCommand[2],'M');
    ASSERT_EQ(dataSize, 3+ 8 +4);  
    int it = 0;
    ASSERT_EQ(inOutData[it++],'(');  //what to expect in command
    ASSERT_EQ(inOutData[it++],'1');  //what to expect in command
    ASSERT_EQ(inOutData[it++],'2');  //what to expect in command
    ASSERT_EQ(inOutData[it++],'3');  //what to expect in command
    ASSERT_EQ(inOutData[it++],'4');  //what to expect in command
    ASSERT_EQ(inOutData[it++],'5');  //what to expect in command
    ASSERT_EQ(inOutData[it++],'6');  //what to expect in command
    ASSERT_EQ(inOutData[it++],'7');  //what to expect in command
    ASSERT_EQ(inOutData[it++],'8');  //what to expect in command

    ASSERT_EQ(inOutData[it++],',');  //what to expect in command
    ASSERT_EQ(inOutData[it++],'1');  //what to expect in command
    ASSERT_EQ(inOutData[it++],'2');  //what to expect in command
    ASSERT_EQ(inOutData[it++],'a');  //what to expect in command
    ASSERT_EQ(inOutData[it++],'c');  //what to expect in command
    ASSERT_EQ(inOutData[it++],')');  //what to expect in command


    ASSERT_EQ(retVal, HKCommDefs::serialErr_None);

}
#endif
#if 1
TEST_F (Serial_R_method_Fixture, readTMV_01)
{
    MockTempMeasurement tm;
    MockSleeper s;
   

    uint8_t inOutCommand[HKCommDefs::commandSize] = 
    {   //command to send
        'R',
        'T',
        'H' 
    };
    uint8_t inOutData[HKCommDefs::commandMaxDataSize] =
    { '0','0','0','1', HKCommDefs::commandEOLSignOnRecieve };

    //TempMeasure::TempRecord TempMeasure::getTempMeasurementRecord(uint16_t howManyRecordsBack)
    EXPECT_CALL(s, getUpTime()).WillOnce(Return(16));

    TempMeasure::TempRecord (10, 1234);
    EXPECT_CALL(tm, getTempMeasurementRecord(0)).
        Times(1).
        WillOnce(Return(TempMeasure::TempRecord (10, 0x123c)));

    //EXPECT_CALL(s, getUpTime()).Times(0);
    
    uint16_t dataSize = 4;
    uint8_t retVal = HKComm::command_R(inOutCommand,inOutData,dataSize);

    //expected results:
    ASSERT_EQ(inOutCommand[0],'V');  //what to expect in command
    ASSERT_EQ(inOutCommand[1],'T');
    ASSERT_EQ(inOutCommand[2],'H');
    ASSERT_EQ(dataSize, 3+ 8 +4);  
    int it = 0;
    ASSERT_EQ(inOutData[it++],'(');  //what to expect in command
    ASSERT_EQ(inOutData[it++],'0');  //what to expect in command
    ASSERT_EQ(inOutData[it++],'0');  //what to expect in command
    ASSERT_EQ(inOutData[it++],'0');  //what to expect in command
    ASSERT_EQ(inOutData[it++],'0');  //what to expect in command
    ASSERT_EQ(inOutData[it++],'0');  //what to expect in command
    ASSERT_EQ(inOutData[it++],'0');  //what to expect in command
    ASSERT_EQ(inOutData[it++],'0');  //what to expect in command
    ASSERT_EQ(inOutData[it++],'6');  //what to expect in command

    ASSERT_EQ(inOutData[it++],',');  //what to expect in command
    ASSERT_EQ(inOutData[it++],'1');  //what to expect in command
    ASSERT_EQ(inOutData[it++],'2');  //what to expect in command
    ASSERT_EQ(inOutData[it++],'3');  //what to expect in command
    ASSERT_EQ(inOutData[it++],'c');  //what to expect in command
    ASSERT_EQ(inOutData[it++],')');  //what to expect in command


    ASSERT_EQ(retVal, HKCommDefs::serialErr_None);

}

#endif

