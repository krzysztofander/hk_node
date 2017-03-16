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
#include "comm_extra_records.h"

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

    dataPut() ={};

    uint8_t retVal = HKComm::command_R(inOutCommand,data(),dataSize());

    //expected results:
    ASSERT_EQ(inOutCommand[0],'V');  //what to expect in command
    ASSERT_EQ(inOutCommand[1],'T');
    ASSERT_EQ(inOutCommand[2],'M');
    ASSERT_EQ(dataSize(), serialResponce_Format4_8_dataLength);  
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
//@brief checks single measurement read
TEST_F (Serial_RTH_method_Fixture, readTMV_01)
{

    dataPut() = { '0','0','0','1'};

    //TempMeasure::TempRecord TempMeasure::getTempMeasurementRecord(uint16_t howManyRecordsBack)
    EXPECT_CALL(mckS, getUpTime())
        .WillOnce(Return(16));

    EXPECT_CALL(mckTm, getTempMeasurementRecord(0))
        .WillOnce(Return(TempMeasure::TempRecord (10, 0x123c)));
   
     uint8_t retVal = HKComm::command_R(inOutCommand,data(),dataSize());

    //expected results:
    ASSERT_EQ(inOutCommand[0],'V');  //what to expect in command
    ASSERT_EQ(inOutCommand[1],'T');
    ASSERT_EQ(inOutCommand[2],'H');
    ASSERT_EQ(dataSize(),serialResponce_Format4_8_dataLength);  
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
//@brief checks the quick access
TEST_F (Serial_RTH_method_Fixture, readTMV_quickAccess)
{
    dataPut() = { };
    EXPECT_CALL(mckS, getUpTime())
        .WillOnce(Return(16));

    EXPECT_CALL(mckTm, getTempMeasurementRecord(0))
        .WillOnce(Return(TempMeasure::TempRecord (10, 0x0111)));

    uint8_t retVal = HKComm::command_R(inOutCommand, data(), dataSize());
    ASSERT_EQ(retVal, HKCommDefs::serialErr_None);
}

//@brief checks the error when there is not enouth chars in data
TEST_F (Serial_RTH_method_Fixture, readTMV_02)
{
    dataPut() = { '1','2','3'};
    uint8_t retVal = HKComm::command_R(inOutCommand, data(), dataSize());
    ASSERT_EQ(retVal, HKCommDefs::serialErr_Number_Uint16ToShort);
}

//@brief checks the error when there is a request for 3 history elements
TEST_F (Serial_RTH_method_Fixture, readTMV_03)
{

    dataPut() ={ '0','0','0','3' };

    EXPECT_CALL(mckS, getUpTime())
        .WillOnce(Return(16));

    EXPECT_CALL(mckTm, getTempMeasurementRecord(0))
        .WillOnce(Return(TempMeasure::TempRecord (10, 0x123c)));
  //      .WillOnce(Return(TempMeasure::TempRecord (8,  0x223c)))
   //     .WillOnce(Return(TempMeasure::TempRecord (1,  0x323c)));

    uint8_t retVal = HKComm::command_R(inOutCommand, data(), dataSize());
    ASSERT_EQ(retVal, HKCommDefs::serialErr_None);

    ASSERT_EQ(inOutCommand[0], 'V');  //what to expect in command
    ASSERT_EQ(inOutCommand[1], 'T');
    ASSERT_EQ(inOutCommand[2], 'H');
    ASSERT_EQ(dataSize(), 1 * (serialResponce_Format4_8_dataLength));

    ASSERT_EQ(HKCommExtraRecordsHDL::recordsIt, 0);
    ASSERT_EQ(HKCommExtraRecordsHDL::totalRecords, 2);


   //     g_serialError = HKCommExtraRecordsHDL::formatedMeasurement(valid, extraRecChars, g_data);
    uint8_t valid;
    int it = 0;

    EXPECT_CALL(mckTm, getTempMeasurementRecord(0))
        .WillOnce(Return(TempMeasure::TempRecord (10, 0x123c)));
    EXPECT_CALL(mckTm, getTempMeasurementRecord(1)). Times(2)
        .WillRepeatedly(Return(TempMeasure::TempRecord (8, 0x223c)));
    dataSize() = 0;
    retVal = HKCommExtraRecordsHDL::formatedMeasurement(valid, dataSize(), data());
    it=0;

    ASSERT_EQ(retVal, HKCommDefs::serialErr_None);
    ASSERT_EQ(dataSize(), 1 * serialResponce_Format4_8_dataLength);
    ASSERT_EQ(data()[it++], '(');  //what to expect in command
    ASSERT_EQ(data()[it++], '0');  //what to expect in command
    ASSERT_EQ(data()[it++], '0');  //what to expect in command
    ASSERT_EQ(data()[it++], '0');  //what to expect in command
    ASSERT_EQ(data()[it++], '0');  //what to expect in command
    ASSERT_EQ(data()[it++], '0');  //what to expect in command
    ASSERT_EQ(data()[it++], '0');  //what to expect in command
    ASSERT_EQ(data()[it++], '0');  //what to expect in command
    ASSERT_EQ(data()[it++], '2');  //what to expect in command
    ASSERT_EQ(data()[it++], ',');  //what to expect in command
    ASSERT_EQ(data()[it++], '2');  //what to expect in command
    ASSERT_EQ(data()[it++], '2');  //what to expect in command
    ASSERT_EQ(data()[it++], '3');  //what to expect in command
    ASSERT_EQ(data()[it++], 'c');  //what to expect in command
    ASSERT_EQ(data()[it++], ')');  //what to expect in command

    EXPECT_CALL(mckTm, getTempMeasurementRecord(1))
        .WillOnce(Return(TempMeasure::TempRecord (8, 0x223c)));
    EXPECT_CALL(mckTm, getTempMeasurementRecord(2))
        .WillRepeatedly(Return(TempMeasure::TempRecord (1, 0x323c)));
    it=0;
    dataSize() = 0;
    retVal = HKCommExtraRecordsHDL::formatedMeasurement(valid, dataSize(), data());

    ASSERT_EQ(retVal, HKCommDefs::serialErr_None);
    ASSERT_EQ(dataSize(), 1 * serialResponce_Format4_8_dataLength);
    ASSERT_EQ(data()[it++], '(');  //what to expect in command
    ASSERT_EQ(data()[it++], '0');  //what to expect in command
    ASSERT_EQ(data()[it++], '0');  //what to expect in command
    ASSERT_EQ(data()[it++], '0');  //what to expect in command
    ASSERT_EQ(data()[it++], '0');  //what to expect in command
    ASSERT_EQ(data()[it++], '0');  //what to expect in command
    ASSERT_EQ(data()[it++], '0');  //what to expect in command
    ASSERT_EQ(data()[it++], '0');  //what to expect in command
    ASSERT_EQ(data()[it++], '7');  //what to expect in command
    ASSERT_EQ(data()[it++], ',');  //what to expect in command
    ASSERT_EQ(data()[it++], '3');  //what to expect in command
    ASSERT_EQ(data()[it++], '2');  //what to expect in command
    ASSERT_EQ(data()[it++], '3');  //what to expect in command
    ASSERT_EQ(data()[it++], 'c');  //what to expect in command
    ASSERT_EQ(data()[it++], ')');  //what to expect in command

    retVal = HKCommExtraRecordsHDL::formatedMeasurement(valid, dataSize(), data());
    ASSERT_EQ(retVal, HKCommDefs::serialErr_None);
    ASSERT_EQ(valid, 0);
}

//@brief checks the error when there is a request for to many history elements
TEST_F (Serial_RTH_method_Fixture, readTMV_04)
{

    dataPut() ={ '1','0','0','3' };

    EXPECT_CALL(mckS, getUpTime())
        .WillOnce(Return(16));

    EXPECT_CALL(mckTm, getTempMeasurementRecord(0))
        .WillOnce(Return(TempMeasure::TempRecord (10, 0x123c)));
    //      .WillOnce(Return(TempMeasure::TempRecord (8,  0x223c)))
    //     .WillOnce(Return(TempMeasure::TempRecord (1,  0x323c)));

    uint8_t retVal = HKComm::command_R(inOutCommand, data(), dataSize());
    ASSERT_EQ(retVal, HKCommDefs::serialErr_None);

    ASSERT_EQ(inOutCommand[0], 'V');  //what to expect in command
    ASSERT_EQ(inOutCommand[1], 'T');
    ASSERT_EQ(inOutCommand[2], 'H');
    ASSERT_EQ(dataSize(), 1 * serialResponce_Format4_8_dataLength);

    ASSERT_EQ(HKCommExtraRecordsHDL::recordsIt, 0);
    ASSERT_EQ(HKCommExtraRecordsHDL::totalRecords, TempMeasure::capacity() - 1);


    uint8_t valid = 1;
    do
    {
        EXPECT_CALL(mckTm, getTempMeasurementRecord(_))
            .WillRepeatedly(Return(TempMeasure::TempRecord (8, 0x223c)));
        dataSize() = 0;
        retVal = HKCommExtraRecordsHDL::formatedMeasurement(valid, dataSize(), data());
   
        ASSERT_EQ(retVal, HKCommDefs::serialErr_None);
        if (valid)
        {
            ASSERT_EQ(dataSize(), 1 * serialResponce_Format4_8_dataLength);
        }
    } while (valid != 0);

}

//@brief checks the error when there is a request for to many history elements
TEST_F (Serial_RTH_method_Fixture, testRespond)
{

    dataPut() ={ '0','0','0','4' };

    EXPECT_CALL(mckS, getUpTime())
        .WillOnce(Return(16));

    EXPECT_CALL(mckTm, getTempMeasurementRecord(0))
        .WillOnce(Return(TempMeasure::TempRecord (10, 0x123c)));
    //      .WillOnce(Return(TempMeasure::TempRecord (8,  0x223c)))
    //     .WillOnce(Return(TempMeasure::TempRecord (1,  0x323c)));

    uint8_t retVal = HKComm::command_R(inOutCommand, data(), dataSize());
    ASSERT_EQ(retVal, HKCommDefs::serialErr_None);

    ASSERT_EQ(inOutCommand[0], 'V');  //what to expect in command
    ASSERT_EQ(inOutCommand[1], 'T');
    ASSERT_EQ(inOutCommand[2], 'H');
    ASSERT_EQ(dataSize(), 1 *serialResponce_Format4_8_dataLength);

    ASSERT_EQ(HKCommExtraRecordsHDL::recordsIt, 0);
    ASSERT_EQ(HKCommExtraRecordsHDL::totalRecords, 3);

    HKComm::g_SerialState =  HKCommDefs::serialState_Respond;
    HKComm::g_serialError =  HKCommDefs::serialErr_None;
    
    std::copy(&data()[0], &data()[NUM_ELS(HKComm::g_data)], &HKComm::g_data[0]);
    HKComm::g_dataIt = dataSize();
    
    EXPECT_CALL(mockSerial, write(_, 3 )).Times(1);

    EXPECT_CALL(mckTm, getTempMeasurementRecord(_))
        .WillRepeatedly(Return(TempMeasure::TempRecord (8, 0x223c)));
    EXPECT_CALL(mockSerial, write(_, serialResponce_Format4_8_dataLength)).Times(4);
    EXPECT_CALL(mockSerial, write(_, NUM_ELS(HKCommDefs::commandEOLOnResponceSequence) )).Times(1);

    HKComm::respondSerial();
    

}

#endif

