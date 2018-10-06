#include "../mork/scene/Scene.h"
#include "../mork/scene/Camera.h"
#include "../mork/math/mat4.h"



#include <gtest/gtest.h>

using mork::SceneNode;
using mork::mat4d;
using mork::vec3d;
using mork::Scene;
using mork::Camera;

class CameraTest : public ::testing::Test {

protected:
    CameraTest();

    virtual ~CameraTest();

    // Code here will be called immediately after the constructor (right
    // before each test).
    virtual void SetUp();

    // Code here will be called immediately after each test (right
    // before the destructor).
    virtual void TearDown();

};



CameraTest::CameraTest()
{

}

CameraTest::~CameraTest()
{

}

void CameraTest::SetUp()
{
}

void CameraTest::TearDown()
{
}

TEST_F(CameraTest, InitTest)
{
    Camera camera;
    
}

