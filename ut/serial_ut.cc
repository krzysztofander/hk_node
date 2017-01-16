#include "gtest/gtest.h" 
#include "gmock/gmock.h"
#include "Arduino.h"
#include "executor.h"
#include "serial.h"
using ::testing::AtLeast;  
using ::testing::Return;
using ::testing::InSequence;
using ::testing::NiceMock;
using ::testing::StrictMock;
using ::testing::_;


TSerial Serial;

class SerialImpl : public Serial_if
{
public:
    virtual  uint8_t  read()
    {
        return 'e';
    }
    virtual  uint8_t available()
    {
        return 0;
    }
    virtual  uint8_t write(const uint8_t * buff, size_t size) 
    {
        return (uint8_t)size;
    }


};


class MockSerial : public Serial_if
{
public:
    MOCK_METHOD0(read, uint8_t());
    MOCK_METHOD0(available, uint8_t());
    MOCK_METHOD2(write, uint8_t(const uint8_t*, size_t));

};

class LooseFn_if
{
public:
    virtual void alert (unsigned char c, bool b) = 0;
    virtual void Executor_setExecutionTime(uint8_t t, uint16_t s) = 0;
};

class MockLooseFn : public LooseFn_if
{
public:
    MOCK_METHOD2(alert, void(uint8_t, bool b));
    MOCK_METHOD2(Executor_setExecutionTime, void(uint8_t, uint16_t));

};

void alert (unsigned char c, bool b)
{

}


void Executor::setExecutionTime(uint8_t t, uint16_t s)
{

}

class SerialFixture : public ::testing::Test
{
public: 
    MockSerial mockSerial;
    InSequence dummy;

    virtual void SetUp()
    {
        Serial.install(&mockSerial);
        HKComm::g_SerialState =  HKComm::serialState_ReadCommand;
        HKComm::g_dataIt = 0;
        HKComm::g_serialError = 0;

    }

};

class NiceSerialFixture : public ::testing::Test
{
public: 
    NiceMock<MockSerial>  mockSerial;
    InSequence dummy;

    virtual void SetUp()
    {
        Serial.install(&mockSerial);
        HKComm::g_SerialState =  HKComm::serialState_ReadCommand;
        HKComm::g_dataIt = 0;
        HKComm::g_serialError = 0;

    }

};


class StrictSerialFixture : public ::testing::Test
{
public: 
    StrictMock<MockSerial>  mockSerial;
    InSequence dummy;

    virtual void SetUp()
    {
        Serial.install(&mockSerial);

        HKComm::g_SerialState =  HKComm::serialState_ReadCommand;
        HKComm::g_dataIt = 0;
        HKComm::g_serialError = 0;
    }

};

class Serial_D_method_Fixture : public SerialFixture
{
public:
    uint8_t dataSize;

};



TEST(Serial, t1)
{
    SerialImpl myS;
    Serial.install(&myS);

    ASSERT_TRUE (1);
}


TEST(Serial, check_dataToUnsignedShort)
{
    //uint8_t HKComm::dataToUnsignedShort(uint8_t offset, const uint8_t (&inData)[commandMaxDataSize], uint16_t & retVal )

    uint8_t inData[HKComm::commandMaxDataSize] = { '1', '2', '3' ,'4', 'f', 'f' ,'f' ,'f' , '0'}; //not really a valid data, should be terminated by EOL
    uint8_t inData2[HKComm::commandMaxDataSize] ={ 'a', 'b', 'a' ,'1', 'c', '2' ,'4' ,'f' , '\n' };

    uint8_t inDataE[HKComm::commandMaxDataSize] ={ 'A', 'B', 'A' ,'1', 'c', '2' ,'4' ,'\n' , '\n' };


    uint16_t retVal;
    uint8_t errorCode;

    errorCode = HKComm::dataToUnsignedShort(0, inData, retVal);
    ASSERT_EQ(retVal, 0x1234u);
    ASSERT_EQ(errorCode, HKComm::serialErr_None);

    errorCode = HKComm::dataToUnsignedShort(4, inData, retVal);
    ASSERT_EQ(retVal, 0xffffu);
    ASSERT_EQ(errorCode, HKComm::serialErr_None);

    errorCode = HKComm::dataToUnsignedShort(3, inData, retVal);
    ASSERT_EQ(retVal, 0x4fffu);
    ASSERT_EQ(errorCode, HKComm::serialErr_None);

    errorCode = HKComm::dataToUnsignedShort(0, inData2, retVal);
    ASSERT_EQ(retVal, 0xaba1u);
    ASSERT_EQ(errorCode, HKComm::serialErr_None);
    // now fail scenarios

    errorCode = HKComm::dataToUnsignedShort(0, inDataE, retVal);
    ASSERT_EQ(errorCode, HKComm::serialErr_IncorrectNumberFormat);

    errorCode = HKComm::dataToUnsignedShort(4, inDataE, retVal);
    ASSERT_EQ(errorCode, HKComm::serialErr_IncorrectNumberFormat);

    errorCode = HKComm::dataToUnsignedShort(5, inData2, retVal);
    ASSERT_EQ(errorCode, HKComm::serialErr_Assert);

    errorCode = HKComm::dataToUnsignedShort(66, inData, retVal);
    ASSERT_EQ(errorCode, HKComm::serialErr_Assert);
    
}


TEST(Serial, initSerial)
{
    SerialImpl myS;
    Serial.install(&myS);

    ASSERT_EQ(HKComm::g_dataIt, 0);
    ASSERT_EQ(HKComm::g_serialError, 0);
    ASSERT_EQ(HKComm::g_SerialState,HKComm:: serialState_ReadCommand);
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
    ASSERT_EQ(HKComm::g_SerialState, HKComm::serialState_ReadCommand);  //state did not change
    ASSERT_EQ(HKComm::g_dataIt, 0);  //no data
    ASSERT_EQ(HKComm::g_serialError, 0);//no error

}

TEST_F(StrictSerialFixture, changeStateToReadData)
{
    int8_t retVal = 0;

    EXPECT_CALL(mockSerial, available())
        .WillOnce(Return(3));
    EXPECT_CALL(mockSerial, read())
        .Times(3)
        .WillRepeatedly(Return('D'));
        

    retVal = HKComm::respondSerial();
    ASSERT_EQ(retVal, 1);  //returned 1, need to change state immediately

    ASSERT_EQ(HKComm::g_SerialState, HKComm::serialState_ReadData);
    ASSERT_EQ(HKComm::g_dataIt, 0);
    ASSERT_EQ(HKComm::g_serialError, 0);

}

TEST_F(StrictSerialFixture, changeStateError_serialErr_eolInCommand01)
{
    int8_t retVal = 0;
 
    EXPECT_CALL(mockSerial, available())
        .WillOnce(Return(3));
    EXPECT_CALL(mockSerial, read())
        .Times(3)
        .WillOnce(Return('D'))
        .WillOnce(Return('D'))
        .WillOnce(Return(HKComm::commandEOLSignOnRecieve));
    
    retVal = HKComm::respondSerial();
    ASSERT_EQ(retVal, 1);

    ASSERT_EQ(HKComm::g_SerialState, HKComm::serialState_Error);
    ASSERT_EQ(HKComm::g_dataIt, 0);
    ASSERT_EQ(HKComm::g_serialError, HKComm::serialErr_eolInCommand);  //end of line encounted in command

}
TEST_F(StrictSerialFixture, changeStateError_serialErr_eolInCommand02)
{
    int8_t retVal = 0;

    EXPECT_CALL(mockSerial, available())
        .WillOnce(Return(3));
    EXPECT_CALL(mockSerial, read())
        .WillOnce(Return('D'))
        .WillOnce(Return(HKComm::commandEOLSignOnRecieve));  //end of line encounted in command

    retVal = HKComm::respondSerial();
    ASSERT_EQ(retVal, 1);
    ASSERT_EQ(HKComm::g_SerialState, HKComm::serialState_Error);
    ASSERT_EQ(HKComm::g_dataIt, 0);
    ASSERT_EQ(HKComm::g_serialError, HKComm::serialErr_eolInCommand);
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
    ASSERT_EQ(HKComm::g_SerialState, HKComm::serialState_ReadData);


    EXPECT_CALL(mockSerial, available()) .WillOnce(Return(1));
    EXPECT_CALL(mockSerial, read() ).WillOnce(Return(HKComm::commandEOLSignOnRecieve));  //EOL in data
    retVal = HKComm::respondSerial();

    ASSERT_EQ(HKComm::g_dataIt, 0);
    ASSERT_EQ(retVal, 1);
    ASSERT_EQ(HKComm::g_SerialState, HKComm::serialState_Action);


    EXPECT_CALL(mockSerial, available()).Times (0);
    EXPECT_CALL(mockSerial, read() ).Times (0);
    retVal = HKComm::respondSerial();
    
    ASSERT_EQ(HKComm::g_dataIt, 0);
    ASSERT_EQ(retVal, 1);
    ASSERT_EQ(HKComm::g_SerialState, HKComm::serialState_Error);
    ASSERT_EQ(HKComm::g_dataIt, 0);
    ASSERT_EQ(HKComm::g_serialError, HKComm::serialErr_UnknownCommand);
}

TEST_F(NiceSerialFixture, changeStateError_serialErr_unknownCommandMoreData)
{
    int8_t retVal = 0;

    HKComm::g_command[0] = '?';
    HKComm::g_SerialState =  HKComm::serialState_ReadData; 
    int i;
    for (i = 0; i < 5; i++)
    {
        //fill in some data
        EXPECT_CALL(mockSerial, available()).WillOnce(Return(1));
        EXPECT_CALL(mockSerial, read()).WillOnce(Return('?'));  //EOL in data
        retVal = HKComm::respondSerial();

        ASSERT_EQ(HKComm::g_dataIt, i+1);
        ASSERT_EQ(retVal, 1);
        ASSERT_EQ(HKComm::g_SerialState, HKComm::serialState_ReadData);
    }
    
    //in serialState_ReadData switch to serialState_Action
    {// end of line finally
        EXPECT_CALL(mockSerial, available()).WillOnce(Return(1));
        EXPECT_CALL(mockSerial, read()).WillOnce(Return(HKComm::commandEOLSignOnRecieve));  //EOL in data
        retVal = HKComm::respondSerial();

        ASSERT_EQ(HKComm::g_dataIt, i /*end of line did not enter*/);
        ASSERT_EQ(retVal, 1);
        ASSERT_EQ(HKComm::g_SerialState, HKComm::serialState_Action);
    }
    //In serialState_Action, switch to serialState_Error
    {
        EXPECT_CALL(mockSerial, available()).Times (0);
        EXPECT_CALL(mockSerial, read()).Times (0);
        retVal = HKComm::respondSerial();


        ASSERT_EQ(retVal, 1);
        ASSERT_EQ(HKComm::g_SerialState, HKComm::serialState_Error);
        ASSERT_EQ(HKComm::g_dataIt, i);   //still data in the buffer to sort out
        ASSERT_EQ(HKComm::g_serialError, HKComm::serialErr_UnknownCommand);
    }
    //In serialState_Error, switch to serialState_ReadCommand:
    {
    //now check if it recovered, e.g. entered the read command  stage
        retVal = HKComm::respondSerial();
        ASSERT_EQ(retVal, 1);
        ASSERT_EQ(HKComm::g_SerialState, HKComm::serialState_ReadCommand);
        ASSERT_EQ(HKComm::g_dataIt, 0);   //data zero after error
        ASSERT_EQ(HKComm::g_serialError, HKComm::serialErr_None);
    }

}

TEST_F(NiceSerialFixture, changeStateError_serialErr_unknownCommandNoEolInData)
{
    int8_t retVal = 0;

    HKComm::g_command[0] = '?';
    HKComm::g_SerialState =  HKComm::serialState_ReadData; 
    int i;
    for (i = 0; i < NUM_ELS(HKComm::g_data) - 1; i++)
    {
        //fill in some data
        EXPECT_CALL(mockSerial, available()).WillOnce(Return(1));
        EXPECT_CALL(mockSerial, read()).WillOnce(Return('?'));  //EOL in data
        retVal = HKComm::respondSerial();

        ASSERT_EQ(HKComm::g_dataIt, i+1);
        ASSERT_EQ(retVal, 1);
        ASSERT_EQ(HKComm::g_SerialState, HKComm::serialState_ReadData);
    }
    //finall call
    //in serialState_ReadData switch to serialState_Error
    {
        EXPECT_CALL(mockSerial, available()).WillOnce(Return(1));
        EXPECT_CALL(mockSerial, read()).WillOnce(Return('?'));  //no EOL in data
        retVal = HKComm::respondSerial();  //swith to error state
        ASSERT_EQ(HKComm::g_dataIt, i /*i got increased already in for loop*/); 
        ASSERT_EQ(retVal, 1);
        ASSERT_EQ(HKComm::g_SerialState, HKComm::serialState_Error);
        ASSERT_EQ(HKComm::g_serialError, HKComm::serialErr_noEolFound);
    }
    //In serialState_Error, switch to serialState_ReadCommand:
    {
    //not check if it recovered, e.g. entered the read command  stage
        retVal = HKComm::respondSerial();
        ASSERT_EQ(retVal, 1);
        ASSERT_EQ(HKComm::g_SerialState, HKComm::serialState_ReadCommand);
        ASSERT_EQ(HKComm::g_dataIt, 0);   //data zero after error
        ASSERT_EQ(HKComm::g_serialError, HKComm::serialErr_None);
    }

}



TEST_F (Serial_D_method_Fixture, echoD)
{

    uint8_t inOutCommand[HKComm::commandSize] = 
    {   //command to send
        'D',
        'E',
        'X' 
    };
    uint8_t inOutData[HKComm::commandMaxDataSize];
    std::fill_n(inOutData, 0,HKComm:: commandMaxDataSize);
    inOutData[0] = HKComm::commandEOLSignOnRecieve;
    
    uint8_t retVal = HKComm::command_D(inOutCommand,inOutData,dataSize);

    //expected results:
    ASSERT_EQ(inOutCommand[0],'D');  //what to expect in command
    ASSERT_EQ(inOutCommand[1],'R');
    ASSERT_EQ(inOutCommand[2],'X');
    ASSERT_EQ(dataSize, 0);  //no data
    ASSERT_EQ(retVal, HKComm::serialErr_None);

}

TEST_F (Serial_D_method_Fixture, unknownD)
{

    uint8_t inOutCommand[HKComm::commandSize] = 
    {   //command to send
        'D',
        '?',
        '?' 
    };
    uint8_t inOutData[HKComm::commandMaxDataSize];
    std::fill_n(inOutData, 0,HKComm:: commandMaxDataSize);
    inOutData[0] = HKComm::commandEOLSignOnRecieve;

    uint8_t retVal = HKComm::command_D(inOutCommand,inOutData,dataSize);

    //expected results:
    ASSERT_EQ(inOutCommand[0],'D');  //what to expect in command
    ASSERT_EQ(inOutCommand[1],'u');
    ASSERT_EQ(inOutCommand[2],'n');
    ASSERT_EQ(dataSize, 0);  //no data
    ASSERT_EQ(retVal, HKComm::serialErr_None);

}

TEST_F(StrictSerialFixture, changeStateToResponse_ForEcho)
{
    int8_t retVal = 0;

    HKComm::g_command[0] = 'D';
    HKComm::g_command[1] = 'E';
    HKComm::g_command[2] = 'X';
    HKComm::g_SerialState =  HKComm::serialState_Action; 

    retVal = HKComm::respondSerial();
    ASSERT_EQ(HKComm::g_SerialState, HKComm::serialState_Respond);
    ASSERT_EQ(retVal, 1);
    ASSERT_EQ(HKComm::g_dataIt, 0);


    {
        EXPECT_CALL(mockSerial, write(_, NUM_ELS(HKComm::g_command))).
            WillOnce(Return(uint8_t(NUM_ELS(HKComm::g_command))));
        EXPECT_CALL(mockSerial, write(_, HKComm::commandEOLSizeOnResponce)).
            WillOnce(Return(HKComm::commandEOLSizeOnResponce));

        retVal = HKComm::respondSerial();
        ASSERT_EQ(HKComm::g_SerialState, HKComm::serialState_ReadCommand);
        ASSERT_EQ(retVal, 1);
        ASSERT_EQ(HKComm::g_serialError, HKComm::serialErr_None);
        ASSERT_EQ(HKComm::g_dataIt, 0);   //data zero after response

    }

}


TEST_F(StrictSerialFixture, testResponse)
{
    int8_t retVal = 0;

    HKComm::g_command[0] = 'D';
    HKComm::g_command[1] = 'E';
    HKComm::g_command[2] = 'X';
    HKComm::g_SerialState =  HKComm::serialState_Respond; 
    HKComm::g_dataIt = 5;

       
    {
        EXPECT_CALL(mockSerial, write(_, NUM_ELS(HKComm::g_command))).
            WillOnce(Return((uint8_t)NUM_ELS(HKComm::g_command)));

        EXPECT_CALL(mockSerial, write(_, 5)).
            WillOnce(Return(5));

        EXPECT_CALL(mockSerial, write(_, HKComm::commandEOLSizeOnResponce)).
            WillOnce(Return(HKComm::commandEOLSizeOnResponce));

        retVal = HKComm::respondSerial();
        ASSERT_EQ(HKComm::g_SerialState, HKComm::serialState_ReadCommand);
        ASSERT_EQ(retVal, 1);
        ASSERT_EQ(HKComm::g_serialError, HKComm::serialErr_None);
        ASSERT_EQ(HKComm::g_dataIt, 0);   //data zero after error

    }

}




