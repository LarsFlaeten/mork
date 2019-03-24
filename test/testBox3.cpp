#include "../mork/math/box3.h"

#include <gtest/gtest.h>



class Box3Test : public ::testing::Test {

protected:
    Box3Test();

    virtual ~Box3Test();

    // Code here will be called immediately after the constructor (right
    // before each test).
    virtual void SetUp();

    // Code here will be called immediately after each test (right
    // before the destructor).
    virtual void TearDown();

};



Box3Test::Box3Test()
{

}

Box3Test::~Box3Test()
{

}

void Box3Test::SetUp()
{
}

void Box3Test::TearDown()
{
}

TEST_F(Box3Test, NormTest)
{
    mork::box3d box(-1, 1, -1, 1, -1, 1);
    ASSERT_EQ(box.norm(), 2);

    mork::box3d box1(-1, 1, 0, 0, -1, 1);
    ASSERT_EQ(box1.norm(), 2);

    mork::box3d box2(-1, 2, 0, 0, 0, 0);
    ASSERT_EQ(box2.norm(), 3);

    mork::box3d box3(1, 2, 0, 0, 0, 0);
    ASSERT_EQ(box3.norm(), 1);

    mork::box3d box4(0, 0, -5, 5, 0, 0);
    ASSERT_EQ(box4.norm(), 10);

    mork::box3d box5(0, 0, 0, 0, -5, 5);
    ASSERT_EQ(box5.norm(), 10);

    mork::box3d box6(1, 2, -100, -98, -5, 5);
    ASSERT_EQ(box6.norm(), 10);

    mork::box3d box7(1, 2, -100, -90, -2, 5);
    ASSERT_EQ(box7.norm(), 10);


}


