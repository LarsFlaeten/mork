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


