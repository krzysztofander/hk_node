#include "gtest/gtest.h" 
#include "gmock/gmock.h"
#include "Arduino.h"
#include "executor.h"
using ::testing::AtLeast;  
using ::testing::Return;
using ::testing::InSequence;
using ::testing::NiceMock;
using ::testing::StrictMock;
using ::testing::_;


void ef(void)
{}

TEST(ExecutorTst, singleExecutor)
{
    int timeSlept = 2;
    int blinkerSleepTime = 7;
    int nextSleepTime = 0;

    Executor::init();
    Executor::setupExecutingFn(Executor::blinker, blinkerSleepTime, ef);
    Executor::rescheduleExecutor(Executor::blinker);

    
    for (int loop = 0; loop < 7; loop++)
    {


        Executor::adjustToElapsedTime(timeSlept);

        uint8_t executor = Executor::giveExecutorToCall();
        // alert(AlertReason_Step3, true);
        if (executor < (uint8_t)Executor::executorsNumber)
        {
            Executor::rescheduleExecutor(executor);
            //execute the executor now... 
            ExecutingFn f = Executor::giveExecutorHandleToCall(executor);
            f();

            nextSleepTime = Executor::getNextSleepTime();
            ASSERT_EQ(nextSleepTime, blinkerSleepTime );

        }
        else
        {
            //nothing to call, it might have been from serial

            nextSleepTime = Executor::getNextSleepTime();
            ASSERT_EQ(nextSleepTime + timeSlept, blinkerSleepTime );

        }

        timeSlept = nextSleepTime;
    }

}





class LooseFn_if
{
public:
    virtual void alert (uint8_t c, bool b) = 0;
    virtual void fake1(void) = 0;
    virtual void fake2(void) = 0;
    virtual void fake3(void) = 0;
};


class MockLooseFn : public LooseFn_if
{
public:
    MOCK_METHOD2( alert, void(uint8_t, bool b));
    MOCK_METHOD0( fake1, void(void));
    MOCK_METHOD0( fake2, void(void));
    MOCK_METHOD0( fake3, void(void));


    static MockLooseFn & instance()
    {
        static MockLooseFn mlf;
        return mlf;
    }

};
void alert (unsigned char c, bool b)
{
    MockLooseFn::instance().alert(c, b);
    assert(0);
}
void fake1(void)
{
    MockLooseFn::instance().fake1();
}
void fake2(void)
{
    MockLooseFn::instance().fake2();
}
void fake3(void)
{
    MockLooseFn::instance().fake3();
}


TEST(ExecutorTst, moreExecutors)
{
    MockLooseFn & mlf = MockLooseFn::instance();

    int timeSlept = 0;
    int fake1SleepTime = 30;
    int fake2SleepTime = 40;
    int fake3SleepTime = 20;
    int nextSleepTime = 0;

    Executor::init();
    Executor::setupExecutingFn(Executor::fakeExecutor1, fake1SleepTime , fake1);
    Executor::setupExecutingFn(Executor::fakeExecutor2, fake2SleepTime , fake2);
    Executor::setupExecutingFn(Executor::fakeExecutor3, fake3SleepTime , fake3);
   // Executor::rescheduleExecutor(Executor::fakeExecutor1);
   // Executor::rescheduleExecutor(Executor::fakeExecutor2);
   // Executor::rescheduleExecutor(Executor::fakeExecutor3);


    // round one. Nothing has slept.  All executors has to run...
    {
        Executor::adjustToElapsedTime(timeSlept);

        uint8_t executor = Executor::giveExecutorToCall();
        ASSERT_EQ(executor, Executor::fakeExecutor1);
        // alert(AlertReason_Step3, true);
        if (executor < (uint8_t)Executor::executorsNumber)
        {
            Executor::rescheduleExecutor(executor);
            ASSERT_EQ(Executor::g_ExecutorsTimeLeft[Executor::fakeExecutor1], fake1SleepTime);
            
            ExecutingFn f = Executor::giveExecutorHandleToCall(executor);
            EXPECT_CALL(mlf, fake1()).Times(1);
            f();
            nextSleepTime = Executor::getNextSleepTime();
            ASSERT_EQ(nextSleepTime, 0  ); //still have 2 pending executors
        }
        else
        {
            ASSERT_TRUE(0); // should not enter here this time
            //nothing to call, it might have been from serial
            nextSleepTime = Executor::getNextSleepTime();
        }
        timeSlept = nextSleepTime;
    }

     // round two. Nothing has slept...
    {
        Executor::adjustToElapsedTime(timeSlept);

        uint8_t executor = Executor::giveExecutorToCall();
        ASSERT_EQ(executor, Executor::fakeExecutor2);
        // alert(AlertReason_Step3, true);
        if (executor < (uint8_t)Executor::executorsNumber)
        {
            Executor::rescheduleExecutor(executor);
            ASSERT_EQ(Executor::g_ExecutorsTimeLeft[Executor::fakeExecutor2], fake2SleepTime);
            
            ExecutingFn f = Executor::giveExecutorHandleToCall(executor);
            EXPECT_CALL(mlf, fake2()).Times(1);
            f();
            nextSleepTime = Executor::getNextSleepTime();
            ASSERT_EQ(nextSleepTime, 0  ); //still have 2 pending executors
        }
        else
        {
            ASSERT_TRUE(0); // should not enter here this time
            //nothing to call, it might have been from serial
            nextSleepTime = Executor::getNextSleepTime();
        }
        timeSlept = nextSleepTime;
    }
    // round 3. goint to sleep for the next time
    {
        Executor::adjustToElapsedTime(timeSlept);

        uint8_t executor = Executor::giveExecutorToCall();
        ASSERT_EQ(executor, Executor::fakeExecutor3);
        // alert(AlertReason_Step3, true);
        if (executor < (uint8_t)Executor::executorsNumber)
        {
            Executor::rescheduleExecutor(executor);
            ASSERT_EQ(Executor::g_ExecutorsTimeLeft[Executor::fakeExecutor3], fake3SleepTime);

            ExecutingFn f = Executor::giveExecutorHandleToCall(executor);
            EXPECT_CALL(mlf, fake3()).Times(1);
            f();
            nextSleepTime = Executor::getNextSleepTime();
            ASSERT_EQ(nextSleepTime, min(fake1SleepTime, min(fake2SleepTime, fake3SleepTime))  ); 
        }
        else
        {
            ASSERT_TRUE(0); // should not enter here this time
                            //nothing to call, it might have been from serial
            nextSleepTime = Executor::getNextSleepTime();
        }
        timeSlept = nextSleepTime;
    }
    //20 to sleep

    //30.40.20 - 20 = 10.20.0
    // round 4. Executor 3 wakes up.
    {
        Executor::adjustToElapsedTime(timeSlept);

        uint8_t executor = Executor::giveExecutorToCall();
        ASSERT_EQ(executor, Executor::fakeExecutor3);
        // alert(AlertReason_Step3, true);
        if (executor < (uint8_t)Executor::executorsNumber)
        {
            Executor::rescheduleExecutor(executor);
            ASSERT_EQ(Executor::g_ExecutorsTimeLeft[Executor::fakeExecutor3], fake3SleepTime);

            ExecutingFn f = Executor::giveExecutorHandleToCall(executor);
            EXPECT_CALL(mlf, fake3()).Times(1);
            f();
            nextSleepTime = Executor::getNextSleepTime();
            ASSERT_EQ(nextSleepTime, min(fake1SleepTime, fake2SleepTime) -  timeSlept  ); 
        }
        else
        {
            ASSERT_TRUE(0); // should not enter here this time
                            //nothing to call, it might have been from serial
            nextSleepTime = Executor::getNextSleepTime();
        }
        timeSlept = nextSleepTime;
    }
    //10 to sleep

    //10.20.20(n) - 10 = 0.10.10
    // round 5. Executor 1 wakes up. Executors 2 will have 10 remaining and 2 will have 10
    {
        Executor::adjustToElapsedTime(timeSlept);

        uint8_t executor = Executor::giveExecutorToCall();
        ASSERT_EQ(executor, Executor::fakeExecutor1);
        // alert(AlertReason_Step3, true);
        if (executor < (uint8_t)Executor::executorsNumber)
        {
            Executor::rescheduleExecutor(executor);
            ASSERT_EQ(Executor::g_ExecutorsTimeLeft[Executor::fakeExecutor1], fake1SleepTime);

            ExecutingFn f = Executor::giveExecutorHandleToCall(executor);
            EXPECT_CALL(mlf, fake1()).Times(1);
            f();
            nextSleepTime = Executor::getNextSleepTime();
            ASSERT_EQ(nextSleepTime, 10  ); 
        }
        else
        {
            ASSERT_TRUE(0); // should not enter here this time
                            //nothing to call, it might have been from serial
            nextSleepTime = Executor::getNextSleepTime();
        }
        timeSlept = nextSleepTime;
    }
    //10 to sleep

    //30n.10.10 - 10 = 20.0.0
    // round 6. Executor 2 wakes up. Executors 3 will have 0 remaining and 1 will have 10
    {
        Executor::adjustToElapsedTime(timeSlept);

        uint8_t executor = Executor::giveExecutorToCall();
        ASSERT_EQ(executor, Executor::fakeExecutor2);
        // alert(AlertReason_Step3, true);
        if (executor < (uint8_t)Executor::executorsNumber)
        {
            Executor::rescheduleExecutor(executor);
            ASSERT_EQ(Executor::g_ExecutorsTimeLeft[Executor::fakeExecutor2], fake2SleepTime);

            ExecutingFn f = Executor::giveExecutorHandleToCall(executor);
            EXPECT_CALL(mlf, fake2()).Times(1);
            f();
            nextSleepTime = Executor::getNextSleepTime();
            ASSERT_EQ(nextSleepTime, 0  ); 
        }
        else
        {
            ASSERT_TRUE(0); // should not enter here this time
                            //nothing to call, it might have been from serial
            nextSleepTime = Executor::getNextSleepTime();
        }
        timeSlept = nextSleepTime;
    }
    //0 to sleep

    //20.40n.0

    {
        Executor::adjustToElapsedTime(timeSlept);

        uint8_t executor = Executor::giveExecutorToCall();
        ASSERT_EQ(executor, Executor::fakeExecutor3);
        // alert(AlertReason_Step3, true);
        if (executor < (uint8_t)Executor::executorsNumber)
        {
            Executor::rescheduleExecutor(executor);
            ASSERT_EQ(Executor::g_ExecutorsTimeLeft[Executor::fakeExecutor3], fake3SleepTime);

            ExecutingFn f = Executor::giveExecutorHandleToCall(executor);
            EXPECT_CALL(mlf, fake3()).Times(1);
            f();
            nextSleepTime = Executor::getNextSleepTime();
            ASSERT_EQ(nextSleepTime, 20  ); 
        }
        else
        {
            ASSERT_TRUE(0); // should not enter here this time
                            //nothing to call, it might have been from serial
            nextSleepTime = Executor::getNextSleepTime();
        }
        timeSlept = nextSleepTime;
    }
    //20 to sleep

   

    //INTERRRUPT!
    timeSlept = 1; 
    //20.40.20n - 1 = 19.39.19

    
    
    {
        Executor::adjustToElapsedTime(timeSlept);

        uint8_t executor = Executor::giveExecutorToCall();
        ASSERT_EQ(executor, uint8_t(255));
        // alert(AlertReason_Step3, true);
        if (executor < (uint8_t)Executor::executorsNumber)
        {
            ASSERT_TRUE(0); // should not enter here this time

        }
        else
        {
                            //nothing to call, it might have been from serial
            nextSleepTime = Executor::getNextSleepTime();
            ASSERT_EQ(nextSleepTime, 19);
            ASSERT_EQ(Executor::g_ExecutorsTimeLeft[Executor::fakeExecutor1], 19);
            ASSERT_EQ(Executor::g_ExecutorsTimeLeft[Executor::fakeExecutor2], 39);
            ASSERT_EQ(Executor::g_ExecutorsTimeLeft[Executor::fakeExecutor3], 19);

        }
        timeSlept = nextSleepTime;
    }



}

TEST(ExecutorTst, periodicInterrupts)
{
    int timeSlept = 1;
    int blinkerSleepTime = 30;
    int nextSleepTime = 0;

    Executor::init();
    Executor::setupExecutingFn(Executor::blinker, blinkerSleepTime, ef);
    Executor::rescheduleExecutor(Executor::blinker);


    for (int loop = 0; loop < 20; loop++)
    {
        timeSlept = 1;

        Executor::adjustToElapsedTime(timeSlept);

        uint8_t executor = Executor::giveExecutorToCall();
        // alert(AlertReason_Step3, true);
        if (executor < (uint8_t)Executor::executorsNumber)
        {
            Executor::rescheduleExecutor(executor);
            //execute the executor now... 
            ExecutingFn f = Executor::giveExecutorHandleToCall(executor);
            f();

            nextSleepTime = Executor::getNextSleepTime();
            ASSERT_EQ(nextSleepTime, blinkerSleepTime );

        }
        else
        {
            //nothing to call, it might have been from serial

            nextSleepTime = Executor::getNextSleepTime();
            ASSERT_EQ(nextSleepTime + timeSlept, blinkerSleepTime - loop);

        }

        timeSlept = nextSleepTime;
    }

}



//fakeExecutor1,
//fakeExecutor2,
//fakeExecutor3,
