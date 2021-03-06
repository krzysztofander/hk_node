#ifndef HK_MOCK_H
#define HK_MOCK_H
#include "gtest/gtest.h" 
#include "gmock/gmock.h"


#include "comm.h"
#include "comm_defs.h"

#include "Arduino.h"
#include "temp_measurement.h"
#include "serial.h"
//using ::testing::AtLeast;  
//using ::testing::Return;
using ::testing::InSequence;
using ::testing::NiceMock;
using ::testing::StrictMock;


class MockSerial : public Serial_if
{
public:
    MOCK_METHOD0(read, uint8_t());
    MOCK_METHOD0(peek, uint8_t());
    MOCK_METHOD0(available, uint8_t());
    MOCK_METHOD2(write, uint8_t(const uint8_t*, size_t));

};

#define FF_GMOCK_METHOD1(Method, ...) \
  MOCK_METHOD1( Method, __VA_ARGS__); \
  friend GMOCK_RESULT_(, __VA_ARGS__)  Method (  GMOCK_ARG_( , 1, __VA_ARGS__) param1 ) \
  { \
      return  instance().Method(param1); \
  } \



template <class MockClass>
class UPMock
{
public:
    UPMock() 
    {
        pInst = static_cast<MockClass * >(this);
    }
    explicit UPMock(MockClass *obj) 
    {
        pInst = obj;
    }
    ~UPMock()
    {
        pInst = 0;
    }

    static MockClass & instance()
    {
        if (!pInst) 
            throw std::exception("getting instance when mock is not instantiated");
        return *pInst;
    }
    static MockClass *pInst;
};


class MockSleeper : public UPMock<MockSleeper>
{
public:
    MOCK_METHOD0( getUpTime, HKTime::UpTime(void));
};



class MockExecutor : public UPMock<MockExecutor>
{
public:
    MOCK_METHOD2(setExecutionTime, void(uint8_t, uint16_t));

};


class MockSupp : public UPMock<MockSupp>
{
public:

    MOCK_METHOD1(blinkLed, void(uint8_t));
    MOCK_METHOD0(isButtonPressed, bool());

};



class MockTempMeasurement : public UPMock<MockTempMeasurement>
{
    
public:
    MOCK_METHOD0(getSingleTempMeasurement, uint16_t(void));
    MOCK_METHOD1(getTempMeasurementRecord, TempMeasure::TempRecord(uint16_t));
 
};



//------------------------------------------------------------------------

class PreableTestFixture : public ::testing::Test
{
public:
    MockSerial mockSerial;

    virtual void SetUp()
    {
        Serial.install(&mockSerial);

       
    }

};

class SerialFixtureComm : public ::testing::Test
{
public:
    virtual void SetUp()
    {
        HKSerial::g_preableState = HKSerial::PreableState::finished;
    }
};

class SerialFixture : public SerialFixtureComm
{
public: 


    MockSerial mockSerial;
    
    virtual void SetUp()
    {
        SerialFixtureComm::SetUp();
        Serial.install(&mockSerial);
        HKComm::g_commState.setState(HKCommState::ESerialState::serialState_ParseCommand);
    }
};


class NiceSerialFixture : public SerialFixtureComm{
public: 
    NiceMock<MockSerial>  mockSerial;
    virtual void SetUp()    {        SerialFixtureComm::SetUp();
        miniInParserReset();
        Serial.install(&mockSerial);              HKComm::g_commState.setState(HKCommState::ESerialState::serialState_ParseCommand);    }
};
class StrictSerialFixture : public SerialFixtureComm
{
public: 
    StrictMock<MockSerial>  mockSerial;
    InSequence dummy;    virtual void SetUp()    {        SerialFixtureComm::SetUp();
        Serial.install(&mockSerial);        HKComm::g_commState.setState(HKCommState::ESerialState::serialState_ParseCommand);    }
};

class SerialBufferHandler : public SerialFixture
{
    /*
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
    */
};

class Serial_D_method_Fixture : public SerialFixture
{
public:
    uint16_t dataSize;

};

class Serial_R_method_Fixture : public SerialBufferHandler
{
public:
    
    //enum
    //{
    //    serialResponce_Format4_8_dataLength = 1+4+1+8+1 + /*for eol:*/ 2

    //};

    MockTempMeasurement mckTm;
    MockSleeper mckS;
/*
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
    */
};

class Serial_RTH_method_Fixture : public Serial_R_method_Fixture
{
public:
  
};
#endif

