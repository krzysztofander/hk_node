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




TEST(Serial, check_dataToUnsignedShort)
{
    //uint8_t HKComm::dataToUnsignedShort(uint8_t offset, const uint8_t (&inData)[commandMaxDataSize], uint16_t & retVal )

    uint8_t inData[HKCommDefs::commandMaxDataSize] = { '1', '2', '3' ,'4', 'f', 'f' ,'f' ,'f' , '0'}; //not really a valid data, should be terminated by EOL
    uint8_t inData2[HKCommDefs::commandMaxDataSize] ={ 'a', 'b', 'a' ,'1', 'c', '2' ,'4' ,'f' , '\n' };

    uint8_t inDataE[HKCommDefs::commandMaxDataSize] ={ 'A', 'B', 'A' ,'1', 'c', '2' ,'4' ,'\n' , '\n' };


    uint16_t retVal;
    uint8_t errorCode;

    errorCode = HKCommCommon::dataToUnsignedShort(0, inData, retVal);
    ASSERT_EQ(retVal, 0x1234u);
    ASSERT_EQ(errorCode, HKCommDefs::serialErr_None);

    errorCode = HKCommCommon::dataToUnsignedShort(4, inData, retVal);
    ASSERT_EQ(retVal, 0xffffu);
    ASSERT_EQ(errorCode, HKCommDefs::serialErr_None);

    errorCode = HKCommCommon::dataToUnsignedShort(3, inData, retVal);
    ASSERT_EQ(retVal, 0x4fffu);
    ASSERT_EQ(errorCode, HKCommDefs::serialErr_None);

    errorCode = HKCommCommon::dataToUnsignedShort(0, inData2, retVal);
    ASSERT_EQ(retVal, 0xaba1u);
    ASSERT_EQ(errorCode, HKCommDefs::serialErr_None);
    // now fail scenarios

    errorCode = HKCommCommon::dataToUnsignedShort(0, inDataE, retVal);
    ASSERT_EQ(errorCode, HKCommDefs::serialErr_Number_IncorrectFormat);

    errorCode = HKCommCommon::dataToUnsignedShort(4, inDataE, retVal);
    ASSERT_EQ(errorCode, HKCommDefs::serialErr_Number_IncorrectFormat);

 
    inData[HKCommDefs::commandMaxDataSize - 5] = '0';
    inData[HKCommDefs::commandMaxDataSize - 4] = '0';
    inData[HKCommDefs::commandMaxDataSize - 3] = '0';
    inData[HKCommDefs::commandMaxDataSize - 2] = '0';
    
    errorCode = HKCommCommon::dataToUnsignedShort(HKCommDefs::commandMaxDataSize - 5, inData, retVal);
    ASSERT_EQ(errorCode, HKCommDefs::serialErr_None);


    errorCode = HKCommCommon::dataToUnsignedShort(HKCommDefs::commandMaxDataSize - 4, inData2, retVal);
    ASSERT_EQ(errorCode, HKCommDefs::serialErr_Assert);

    errorCode = HKCommCommon::dataToUnsignedShort(HKCommDefs::commandMaxDataSize + 66, inData, retVal);
    ASSERT_EQ(errorCode, HKCommDefs::serialErr_Assert);
    
}


TEST(Serial, initSerial)
{
    ASSERT_EQ(HKComm::g_dataIt, 0);
    ASSERT_EQ(HKComm::g_serialError, 0);
    ASSERT_EQ(HKComm::g_SerialState,HKCommDefs:: serialState_ReadCommand);
}


TEST_F(StrictSerialFixture, notEnoutDataAvailable)
{
    int8_t retVal = 0;
    //check if function exits when not enouth data available
    //state is read command
    EXPECT_CALL(mockSerial, available())
        .WillOnce(Return(2))
        .WillRepeatedly(Return(0));
    EXPECT_CALL(mockSerial, read())
        .Times(0);
    retVal = HKComm::respondSerial();

    EXPECT_CALL(mockSerial, available())
        .WillOnce(Return(0));
    retVal += HKComm::respondSerial();

    EXPECT_CALL(mockSerial, available())
        .WillOnce(Return(0));
    retVal += HKComm::respondSerial();

    ASSERT_EQ(retVal, 0); //returned 0, no need to change state
    ASSERT_EQ(HKComm::g_SerialState, HKCommDefs::serialState_ReadCommand);  //state did not change
    ASSERT_EQ(HKComm::g_dataIt, 0);  //no data
    ASSERT_EQ(HKComm::g_serialError, 0);//no error

}

TEST_F(NiceSerialFixture, changeStateToReadData)
{
    int8_t retVal = 0;

    EXPECT_CALL(mockSerial, available())
        .WillOnce(Return(3));
    EXPECT_CALL(mockSerial, read())
        .Times(3)
        .WillRepeatedly(Return('D'));
        

    retVal = HKComm::respondSerial();
    ASSERT_EQ(retVal, 1);  //returned 1, need to change state immediately

    ASSERT_EQ(HKComm::g_SerialState, HKCommDefs::serialState_ReadData);
    ASSERT_EQ(HKComm::g_dataIt, 0);
    ASSERT_EQ(HKComm::g_serialError, 0);

}

TEST_F(StrictSerialFixture, changeStateToReadData_preambleAdded)
{
    int8_t retVal = 0;


    EXPECT_CALL(mockSerial, available())
        .WillOnce(Return(3));
    EXPECT_CALL(mockSerial, peek())
        .Times(1)
        .WillRepeatedly(Return('!'));
    EXPECT_CALL(mockSerial, read())
        .Times(1)
        .WillRepeatedly(Return('D'));

    retVal = HKComm::respondSerial();
    ASSERT_EQ(retVal, 0);  //returned 0, no change

    
    EXPECT_CALL(mockSerial, available())
        .WillOnce(Return(3));
    EXPECT_CALL(mockSerial, peek())
        .Times(1)
        .WillRepeatedly(Return('D'));
    EXPECT_CALL(mockSerial, read())
        .Times(3)
        .WillRepeatedly(Return('D'));


    retVal = HKComm::respondSerial();
    ASSERT_EQ(retVal, 1);  //returned 1, need to change state immediately

    ASSERT_EQ(HKComm::g_SerialState, HKCommDefs::serialState_ReadData);
    ASSERT_EQ(HKComm::g_dataIt, 0);
    ASSERT_EQ(HKComm::g_serialError, 0);

}


TEST_F(StrictSerialFixture, changeStateError_serialErr_eolInCommand01)
{
    int8_t retVal = 0;
 
    EXPECT_CALL(mockSerial, available())
        .WillOnce(Return(3));

    EXPECT_CALL(mockSerial, peek()). Times(1).WillOnce(Return('D'));

    EXPECT_CALL(mockSerial, read())
        .Times(3)
        .WillOnce(Return('D'))
        .WillOnce(Return('D'))
        .WillOnce(Return(HKCommDefs::commandEOLSignOnRecieve));
    
    retVal = HKComm::respondSerial();
    ASSERT_EQ(retVal, 1);

    ASSERT_EQ(HKComm::g_SerialState, HKCommDefs::serialState_Error);
    ASSERT_EQ(HKComm::g_dataIt, 0);
    ASSERT_EQ(HKComm::g_serialError, HKCommDefs::serialErr_eolInCommand);  //end of line encounted in command

}
TEST_F(StrictSerialFixture, changeStateError_serialErr_eolInCommand02)
{
    int8_t retVal = 0;

    EXPECT_CALL(mockSerial, available())
        .WillOnce(Return(3));
    EXPECT_CALL(mockSerial, peek()).WillOnce(Return('D'));
    EXPECT_CALL(mockSerial, read())
        .WillOnce(Return('D'))
        .WillOnce(Return(HKCommDefs::commandEOLSignOnRecieve));  //end of line encounted in command

    retVal = HKComm::respondSerial();
    ASSERT_EQ(retVal, 1);
    ASSERT_EQ(HKComm::g_SerialState, HKCommDefs::serialState_Error);
    ASSERT_EQ(HKComm::g_dataIt, 0);
    ASSERT_EQ(HKComm::g_serialError, HKCommDefs::serialErr_eolInCommand);
}


//checking what happen when recieved unknown command...
TEST_F(NiceSerialFixture, changeStateError_serialErr_unknownCommand)
{
    int8_t retVal = 0;

    EXPECT_CALL(mockSerial, available())
        .WillOnce(Return(3));
    EXPECT_CALL(mockSerial, read())
        .WillOnce(Return('?'))
        .WillOnce(Return('?'))
        .WillOnce(Return('?'));  //Command is correct, but unknown

   
    retVal = HKComm::respondSerial();
    ASSERT_EQ(HKComm::g_dataIt, 0);
    ASSERT_EQ(retVal, 1);
    ASSERT_EQ(HKComm::g_SerialState, HKCommDefs::serialState_ReadData);


    EXPECT_CALL(mockSerial, available()) .WillOnce(Return(1));
    EXPECT_CALL(mockSerial, read() ).WillOnce(Return(HKCommDefs::commandEOLSignOnRecieve));  //EOL in data
    retVal = HKComm::respondSerial();

    ASSERT_EQ(HKComm::g_dataIt, 0);
    ASSERT_EQ(retVal, 1);
    ASSERT_EQ(HKComm::g_SerialState, HKCommDefs::serialState_Action);


    EXPECT_CALL(mockSerial, available()).Times (0);
    EXPECT_CALL(mockSerial, read() ).Times (0);
    retVal = HKComm::respondSerial();
    
    ASSERT_EQ(HKComm::g_dataIt, 0);
    ASSERT_EQ(retVal, 1);
    ASSERT_EQ(HKComm::g_SerialState, HKCommDefs::serialState_Error);
    ASSERT_EQ(HKComm::g_dataIt, 0);
    ASSERT_EQ(HKComm::g_serialError, HKCommDefs::serialErr_UnknownCommand);
}

TEST_F(NiceSerialFixture, changeStateError_serialErr_unknownCommandMoreData)
{
    int8_t retVal = 0;

    HKComm::g_command[0] = '?';
    HKComm::g_SerialState =  HKCommDefs::serialState_ReadData; 
    int i;
    for (i = 0; i < 5; i++)
    {
        //fill in some data
        EXPECT_CALL(mockSerial, available()).WillOnce(Return(1));
        EXPECT_CALL(mockSerial, read()).WillOnce(Return('?'));  //EOL in data
        retVal = HKComm::respondSerial();

        ASSERT_EQ(HKComm::g_dataIt, i+1);
        ASSERT_EQ(retVal, 1);
        ASSERT_EQ(HKComm::g_SerialState, HKCommDefs::serialState_ReadData);
    }
    
    //in serialState_ReadData switch to serialState_Action
    {// end of line finally
        EXPECT_CALL(mockSerial, available()).WillOnce(Return(1));
        EXPECT_CALL(mockSerial, read()).WillOnce(Return(HKCommDefs::commandEOLSignOnRecieve));  //EOL in data
        retVal = HKComm::respondSerial();

        ASSERT_EQ(HKComm::g_dataIt, i /*end of line did not enter*/);
        ASSERT_EQ(retVal, 1);
        ASSERT_EQ(HKComm::g_SerialState, HKCommDefs::serialState_Action);
    }
    //In serialState_Action, switch to serialState_Error
    {
        EXPECT_CALL(mockSerial, available()).Times (0);
        EXPECT_CALL(mockSerial, read()).Times (0);
        retVal = HKComm::respondSerial();


        ASSERT_EQ(retVal, 1);
        ASSERT_EQ(HKComm::g_SerialState, HKCommDefs::serialState_Error);
        ASSERT_EQ(HKComm::g_dataIt, i);   //still data in the buffer to sort out
        ASSERT_EQ(HKComm::g_serialError, HKCommDefs::serialErr_UnknownCommand);
    }
    //In serialState_Error, switch to serialState_ReadCommand:
    {
    //now check if it recovered, e.g. entered the responce with error command  stage
        retVal = HKComm::respondSerial();
        ASSERT_EQ(retVal, 1);
        ASSERT_EQ(HKComm::g_SerialState, HKCommDefs::serialState_Respond);
        ASSERT_EQ(HKComm::g_dataIt, 4);   //error responce
        ASSERT_EQ(HKComm::g_serialError, HKCommDefs::serialErr_None);
    }

}

TEST_F(NiceSerialFixture, changeStateError_serialErr_unknownCommandNoEolInData)
{
    int8_t retVal = 0;

    HKComm::g_command[0] = '?';
    HKComm::g_SerialState =  HKCommDefs::serialState_ReadData; 
    int i;
    for (i = 0; i < NUM_ELS(HKComm::g_data) - 1; i++)
    {
        //fill in some data
        EXPECT_CALL(mockSerial, available()).WillOnce(Return(1));
        EXPECT_CALL(mockSerial, read()).WillOnce(Return('?'));  //EOL in data
        retVal = HKComm::respondSerial();

        ASSERT_EQ(HKComm::g_dataIt, i+1);
        ASSERT_EQ(retVal, 1);
        ASSERT_EQ(HKComm::g_SerialState, HKCommDefs::serialState_ReadData);
    }
    //finall call
    //in serialState_ReadData switch to serialState_Error
    {
        EXPECT_CALL(mockSerial, available()).WillOnce(Return(1));
        EXPECT_CALL(mockSerial, read()).WillOnce(Return('?'));  //no EOL in data
        retVal = HKComm::respondSerial();  //swith to error state
        ASSERT_EQ(HKComm::g_dataIt, i /*i got increased already in for loop*/); 
        ASSERT_EQ(retVal, 1);
        ASSERT_EQ(HKComm::g_SerialState, HKCommDefs::serialState_Error);
        ASSERT_EQ(HKComm::g_serialError, HKCommDefs::serialErr_noEolFound);
    }
    //In serialState_Error, switch to serialState_ReadCommand:
    {
    //not check if it recovered, e.g. entered the read command  stage
        retVal = HKComm::respondSerial();
        ASSERT_EQ(retVal, 1);
        ASSERT_EQ(HKComm::g_SerialState, HKCommDefs::serialState_Respond);
    }

}



TEST_F (Serial_D_method_Fixture, echoD)
{

    uint8_t inOutCommand[HKCommDefs::commandSize] = 
    {   //command to send
        'D',
        'E',
        'X' 
    };
    uint8_t inOutData[HKCommDefs::commandMaxDataSize];
    std::fill_n(inOutData, 0,HKCommDefs:: commandMaxDataSize);
    inOutData[0] = HKCommDefs::commandEOLSignOnRecieve;
    
    uint8_t retVal = HKComm::command_D(inOutCommand,inOutData,dataSize);

    //expected results:
    ASSERT_EQ(inOutCommand[0],'D');  //what to expect in command
    ASSERT_EQ(inOutCommand[1],'R');
    ASSERT_EQ(inOutCommand[2],'X');
    ASSERT_EQ(dataSize, 0);  //no data
    ASSERT_EQ(retVal, HKCommDefs::serialErr_None);

}

TEST_F (Serial_D_method_Fixture, unknownD)
{

    uint8_t inOutCommand[HKCommDefs::commandSize] = 
    {   //command to send
        'D',
        '?',
        '?' 
    };
    uint8_t inOutData[HKCommDefs::commandMaxDataSize];
    std::fill_n(inOutData, 0,HKCommDefs:: commandMaxDataSize);
    inOutData[0] = HKCommDefs::commandEOLSignOnRecieve;

    uint8_t retVal = HKComm::command_D(inOutCommand,inOutData,dataSize);

    //expected results:
    ASSERT_EQ(inOutCommand[0],'D');  //what to expect in command
    ASSERT_EQ(inOutCommand[1],'u');
    ASSERT_EQ(inOutCommand[2],'n');
    ASSERT_EQ(dataSize, 0);  //no data
    ASSERT_EQ(retVal, HKCommDefs::serialErr_None);

}

TEST_F(StrictSerialFixture, changeStateToResponse_ForEcho)
{
    int8_t retVal = 0;

    HKComm::g_command[0] = 'D';
    HKComm::g_command[1] = 'E';
    HKComm::g_command[2] = 'X';
    HKComm::g_SerialState =  HKCommDefs::serialState_Action; 

    retVal = HKComm::respondSerial();
    ASSERT_EQ(HKComm::g_SerialState, HKCommDefs::serialState_Respond);
    ASSERT_EQ(retVal, 1);
    ASSERT_EQ(HKComm::g_dataIt, 0);


    {
        EXPECT_CALL(mockSerial, write(_, NUM_ELS(HKComm::g_command))).
            WillOnce(Return(uint8_t(NUM_ELS(HKComm::g_command))));
        EXPECT_CALL(mockSerial, write(_, HKCommDefs::commandEOLSizeOnResponce)).
            WillOnce(Return(HKCommDefs::commandEOLSizeOnResponce));

        retVal = HKComm::respondSerial();
        ASSERT_EQ(HKComm::g_SerialState, HKCommDefs::serialState_ReadCommand);
        ASSERT_EQ(retVal, 1);
        ASSERT_EQ(HKComm::g_serialError, HKCommDefs::serialErr_None);
        ASSERT_EQ(HKComm::g_dataIt, 0);   //data zero after response

    }

}


TEST_F(StrictSerialFixture, testResponse)
{
    int8_t retVal = 0;

    HKComm::g_command[0] = 'D';
    HKComm::g_command[1] = 'E';
    HKComm::g_command[2] = 'X';
    HKComm::g_SerialState =  HKCommDefs::serialState_Respond; 
    HKComm::g_dataIt = 5;

       
    {
        EXPECT_CALL(mockSerial, write(_, NUM_ELS(HKComm::g_command))).
            WillOnce(Return((uint8_t)NUM_ELS(HKComm::g_command)));

        EXPECT_CALL(mockSerial, write(_, 5)).
            WillOnce(Return(5));

        EXPECT_CALL(mockSerial, write(_, HKCommDefs::commandEOLSizeOnResponce)).
            WillOnce(Return(HKCommDefs::commandEOLSizeOnResponce));

        retVal = HKComm::respondSerial();
        ASSERT_EQ(HKComm::g_SerialState, HKCommDefs::serialState_ReadCommand);
        ASSERT_EQ(retVal, 1);
        ASSERT_EQ(HKComm::g_serialError, HKCommDefs::serialErr_None);
        ASSERT_EQ(HKComm::g_dataIt, 0);   //data zero after error

    }

}


