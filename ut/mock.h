#ifndef HK_MOCK_H
#define HK_MOCK_H
#include "gtest/gtest.h" 
#include "gmock/gmock.h"


#include "comm.h"
#include "comm_defs.h"

#include "Arduino.h"
#include "temp_measurement.h"

//using ::testing::AtLeast;  
//using ::testing::Return;
using ::testing::InSequence;
using ::testing::NiceMock;
using ::testing::StrictMock;

class MockSleeper
{
public:
    MOCK_METHOD0( getUpTime, HKTime::UpTime(void));
   
    MockSleeper()
    {
        pInst = this;
    }
    ~MockSleeper()
    {
        pInst = 0;
    }

    static MockSleeper & instance()
    {
        if (!pInst) 
            throw std::exception("getting instance when mock is not instantiated");
        return *pInst;
    }
private:
    static MockSleeper *pInst;
};

class MockSerial : public Serial_if
{
public:
    MOCK_METHOD0(read, uint8_t());
    MOCK_METHOD0(peek, uint8_t());
    MOCK_METHOD0(available, uint8_t());
    MOCK_METHOD2(write, uint8_t(const uint8_t*, size_t));

};

class MockLooseFn 
{
public:
    MOCK_METHOD2(alert, void(uint8_t, bool b));
    static MockLooseFn & instance()
    {
        static MockLooseFn  f;
        return f;
    }
};

class MockExecutor
{
public:
    MOCK_METHOD2(setExecutionTime, void(uint8_t, uint16_t));

    MockExecutor()
    {
        pInst = this;
    }
    ~MockExecutor()
    {
        pInst = 0;
    }

    static MockExecutor & instance()
    {
        if (!pInst) 
            throw std::exception("getting instance when mock is not instantiated");
        return *pInst;   
    }

private:
    static MockExecutor *pInst;


};


class MockTempMeasurement
{
public:
    MOCK_METHOD0(getSingleTempMeasurement, uint16_t(void));
    MOCK_METHOD1(getTempMeasurementRecord, TempMeasure::TempRecord(uint16_t));
    
    MockTempMeasurement()
    {
        pInst = this;
    }
    ~MockTempMeasurement()
    {
        pInst = 0;
    }

    static MockTempMeasurement & instance()
    {
        if (!pInst) 
            throw std::exception("getting instance when mock is not instantiated");
        return *pInst;
    }
private:
    static MockTempMeasurement *pInst;

};


//------------------------------------------------------------------------

class SerialFixtureComm : public ::testing::Test
{
public:
    enum
    {
        serialResponce_OK_dataLength = 3

    };

};

class SerialFixture : public SerialFixtureComm
{
public: 


    MockSerial mockSerial;


    virtual void SetUp()
    {
        Serial.install(&mockSerial);
        HKComm::g_SerialState =  HKCommDefs::serialState_ReadCommand;
        HKComm::g_dataIt = 0;
        HKComm::g_serialError = 0;
    }
};

class NiceSerialFixture : public SerialFixtureComm{
public: 
    NiceMock<MockSerial>  mockSerial;
    InSequence dummy;    virtual void SetUp()    {
        Serial.install(&mockSerial);        HKComm::g_SerialState =  HKCommDefs::serialState_ReadCommand;        HKComm::g_dataIt = 0;        HKComm::g_serialError = 0;    }
};
class StrictSerialFixture : public SerialFixtureComm
{
public: 
    StrictMock<MockSerial>  mockSerial;
    InSequence dummy;    virtual void SetUp()    {
        Serial.install(&mockSerial);        HKComm::g_SerialState =  HKCommDefs::serialState_ReadCommand;        HKComm::g_dataIt = 0;        HKComm::g_serialError = 0;    }};

class SerialBufferHandler : public SerialFixture
{
public:
    class SerialBufferHldrInt
    {
    public:
        typedef    uint8_t (& InOutRef)[HKCommDefs::commandMaxDataSize];
        
        uint8_t inOutData[HKCommDefs::commandMaxDataSize];
        uint16_t dataSize;

        SerialBufferHldrInt()
        {
            std::fill_n(inOutData, 0,HKCommDefs:: commandMaxDataSize);
            inOutData[0] = HKCommDefs::commandEOLSignOnRecieve;
        }


        SerialBufferHldrInt & operator=(std::initializer_list<uint8_t>  init)
        {
            put(init);
            return *this;
        }

        void put(std::initializer_list<uint8_t>  init)
        {
            if (init.size() >= HKCommDefs::commandMaxDataSize -1)
            {
                throw std::exception("initializer to long for data");
            }
            std::copy(init.begin(), init.end(), inOutData);
            dataSize = (uint16_t)init.size();
            inOutData[dataSize] = HKCommDefs::commandEOLSignOnRecieve;
            //termination sign (\r) does not count
        }
    }rcData;
};

class Serial_D_method_Fixture : public SerialFixture
{
public:
    uint16_t dataSize;

};

class Serial_R_method_Fixture : public SerialBufferHandler
{
public:
    
    enum
    {
        serialResponce_Format4_8_dataLength = 1+4+1+8+1 + /*for eol:*/ 2

    };

    MockTempMeasurement mckTm;
    MockSleeper mckS;

    uint16_t & dataSize()
    {
        return rcData.dataSize;
    }
    SerialBufferHldrInt::InOutRef data()
    {
        return rcData.inOutData;
    }
    void dataPut(std::initializer_list<uint8_t>  init)
    {
        return rcData.put(init);
    }
    SerialBufferHldrInt & dataPut()
    {
        return rcData;
    }
};

class Serial_RTH_method_Fixture : public Serial_R_method_Fixture
{
public:
    uint8_t inOutCommand[HKCommDefs::commandSize] =
    {   //command to send
        'R',
        'T',
        'H'
    };
};
#endif

