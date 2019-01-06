#include "../mork/util/Util.h"
#include "../mork/core/Log.h"

#include <gtest/gtest.h>

using mork::debug_logger;

class UtilTest : public ::testing::Test {

protected:
    UtilTest();

    virtual ~UtilTest();

    // Code here will be called immediately after the constructor (right
    // before each test).
    virtual void SetUp();

    // Code here will be called immediately after each test (right
    // before the destructor).
    virtual void TearDown();

};



UtilTest::UtilTest()
{

}

UtilTest::~UtilTest()
{

}

void UtilTest::SetUp()
{
}

void UtilTest::TearDown()
{
}

TEST_F(UtilTest, Test)
{
    std::string test = mork::random_string(45);

    ASSERT_EQ(test.length(), 45);
}

TEST_F(UtilTest, Test2)
{
    std::string test = mork::random_string(12);

    debug_logger(test);

    ASSERT_EQ(test.length(), 12);
}


