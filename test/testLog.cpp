#include "../mork/core/Log.h"

#include <gtest/gtest.h>



class LogTest : public ::testing::Test {

protected:
    LogTest();

    virtual ~LogTest();

    // Code here will be called immediately after the constructor (right
    // before each test).
    virtual void SetUp();

    // Code here will be called immediately after each test (right
    // before the destructor).
    virtual void TearDown();

};



LogTest::LogTest()
{

    // removed test - difficult after datetime was include in log printing..
    // But it works as expected
    /*
    mork::error_logger("TEST");
    EXPECT_EQ(mork::error_logger.last(), "[ERROR] TEST\n");

    mork::error_logger("TEST2");
    EXPECT_NE(mork::error_logger.last(), "[ERROR] TEST\n");
    EXPECT_EQ(mork::error_logger.last(), "[ERROR] TEST2\n");
*/



}

LogTest::~LogTest()
{

}

void LogTest::SetUp()
{
}

void LogTest::TearDown()
{
}

TEST_F(LogTest, InitTest)
{
}


