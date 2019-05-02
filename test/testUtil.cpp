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

TEST_F(UtilTest, TestString2Vec3d)
{
    mork::vec3d v = mork::string2vec3d("1,2,3");

    ASSERT_EQ(v, mork::vec3d(1, 2, 3));

    v = mork::string2vec3d("1,  2,      3");
    ASSERT_EQ(v, mork::vec3d(1, 2, 3));

    v = mork::string2vec3d("-1,  2.34567, 3.12567");
    ASSERT_EQ(v, mork::vec3d(-1, 2.34567, 3.12567));

    v = mork::string2vec3d("-1.0e4,  2.34567e-4, 3.12567e0");
    ASSERT_EQ(v, mork::vec3d(-1.0e4,  2.34567e-4, 3.12567e0));
 

    ASSERT_THROW(v = mork::string2vec3d("7000,3, 1.0, 1.0"), std::invalid_argument);


    ASSERT_THROW(v = mork::string2vec3d("1,2"), std::invalid_argument);

    ASSERT_THROW(v = mork::string2vec3d("1,a, 3"), std::invalid_argument);


}


