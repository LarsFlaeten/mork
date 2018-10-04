#include "../mork/math/mat4.h"

#include <gtest/gtest.h>



class Mat4Test : public ::testing::Test {

protected:
    Mat4Test();

    virtual ~Mat4Test();

    // Code here will be called immediately after the constructor (right
    // before each test).
    virtual void SetUp();

    // Code here will be called immediately after each test (right
    // before the destructor).
    virtual void TearDown();

};



Mat4Test::Mat4Test()
{

}

Mat4Test::~Mat4Test()
{

}

void Mat4Test::SetUp()
{
}

void Mat4Test::TearDown()
{
}

TEST_F(Mat4Test, ScaleTest)
{

    mork::mat4d scale = mork::mat4d::scale(mork::vec3d(2.0, 3.0, 4.0));

    mork::mat4d scale2 = mork::mat4d(2.0, 0.0, 0.0, 0.0,
                                    0.0, 3.0, 0.0, 0.0,
                                    0.0, 0.0, 4.0, 0.0,
                                    0.0, 0.0, 0.0, 1.0);

    ASSERT_EQ(scale, scale2);
}



TEST_F(Mat4Test, ScaleTranslate)
{

    mork::mat4d trans = mork::mat4d::translate(mork::vec3d(1.0, 2.0, 3.0));
    mork::mat4d scale = mork::mat4d::scale(mork::vec3d(2, 3, 4));

    mork::mat4d prod = mork::mat4d( 2, 0, 0, 1,
                                    0, 3, 0, 2,
                                    0, 0, 4, 3,
                                    0, 0, 0, 1);

    ASSERT_EQ(trans*scale,  prod);






}


