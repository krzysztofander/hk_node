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
    uint8_t inOutCommand[HKCommDefs::commandSize] = 
    {   //command to send
        'R',
        'T',
        'M' 
    };
 

    EXPECT_CALL(mckTm, getSingleTempMeasurement()).Times(1).WillOnce(Return(0x12ac));
    EXPECT_CALL(mckS, getUpTime()).WillOnce(Return(0xabcd12345678ll));

    uint8_t retVal = HKComm::command_R(inOutCommand,data(),dataSize());

    //expected results:
    ASSERT_EQ(inOutCommand[0],'V');  //what to expect in command
    ASSERT_EQ(inOutCommand[1],'T');
    ASSERT_EQ(inOutCommand[2],'M');
    ASSERT_EQ(dataSize(), 3+ 8 +4);  
    int it = 0;
    ASSERT_EQ(data()[it++],'(');  //what to expect in command
    ASSERT_EQ(data()[it++],'1');  //what to expect in command
    ASSERT_EQ(data()[it++],'2');  //what to expect in command
    ASSERT_EQ(data()[it++],'3');  //what to expect in command
    ASSERT_EQ(data()[it++],'4');  //what to expect in command
    ASSERT_EQ(data()[it++],'5');  //what to expect in command
    ASSERT_EQ(data()[it++],'6');  //what to expect in command
    ASSERT_EQ(data()[it++],'7');  //what to expect in command
    ASSERT_EQ(data()[it++],'8');  //what to expect in command
           
    ASSERT_EQ(data()[it++],',');  //what to expect in command
    ASSERT_EQ(data()[it++],'1');  //what to expect in command
    ASSERT_EQ(data()[it++],'2');  //what to expect in command
    ASSERT_EQ(data()[it++],'a');  //what to expect in command
    ASSERT_EQ(data()[it++],'c');  //what to expect in command
    ASSERT_EQ(data()[it++],')');  //what to expect in command


    ASSERT_EQ(retVal, HKCommDefs::serialErr_None);

}
#endif
#if 1
TEST_F (Serial_R_method_Fixture, readTMV_01)
{

    uint8_t inOutCommand[HKCommDefs::commandSize] = 
    {   //command to send
        'R',
        'T',
        'H' 
    };

    rcData = { '0','0','0','1'};

    //TempMeasure::TempRecord TempMeasure::getTempMeasurementRecord(uint16_t howManyRecordsBack)
    EXPECT_CALL(mckS, getUpTime())
        .WillOnce(Return(16));

    TempMeasure::TempRecord (10, 1234);
    EXPECT_CALL(mckTm, getTempMeasurementRecord(0))
        .WillOnce(Return(TempMeasure::TempRecord (10, 0x123c)));
   
     uint8_t retVal = HKComm::command_R(inOutCommand,data(),dataSize());

    //expected results:
    ASSERT_EQ(inOutCommand[0],'V');  //what to expect in command
    ASSERT_EQ(inOutCommand[1],'T');
    ASSERT_EQ(inOutCommand[2],'H');
    ASSERT_EQ(dataSize(), 3+ 8 +4);  
    int it = 0;
    ASSERT_EQ(data()[it++],'(');  //what to expect in command
    ASSERT_EQ(data()[it++],'0');  //what to expect in command
    ASSERT_EQ(data()[it++],'0');  //what to expect in command
    ASSERT_EQ(data()[it++],'0');  //what to expect in command
    ASSERT_EQ(data()[it++],'0');  //what to expect in command
    ASSERT_EQ(data()[it++],'0');  //what to expect in command
    ASSERT_EQ(data()[it++],'0');  //what to expect in command
    ASSERT_EQ(data()[it++],'0');  //what to expect in command
    ASSERT_EQ(data()[it++],'6');  //what to expect in command
            
    ASSERT_EQ(data()[it++],',');  //what to expect in command
    ASSERT_EQ(data()[it++],'1');  //what to expect in command
    ASSERT_EQ(data()[it++],'2');  //what to expect in command
    ASSERT_EQ(data()[it++],'3');  //what to expect in command
    ASSERT_EQ(data()[it++],'c');  //what to expect in command
    ASSERT_EQ(data()[it++],')');  //what to expect in command


    ASSERT_EQ(retVal, HKCommDefs::serialErr_None);

}

TEST_F (Serial_R_method_Fixture, readTMV_02)
{
    uint8_t inOutCommand[HKCommDefs::commandSize] =
    {   //command to send
        'R',
        'T',
        'H'
    };
    setData() = {  HKCommDefs::commandEOLSignOnRecieve };
    dataSize() = 3;
    uint8_t retVal = HKComm::command_R(inOutCommand, data(), dataSize());
}
    
#endif

