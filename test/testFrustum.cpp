#include "../mork/scene/Frustum.h"
#include "../mork/scene/Scene.h"
#include "../mork/scene/SceneNode.h"
#include "../mork/math/mat4.h"
#include "../mork/scene/Camera.h"


#include <gtest/gtest.h>

using mork::SceneNode;
using mork::mat4d;
using mork::vec3d;
using mork::vec4d;
using mork::Scene;
using mork::Frustum;
using mork::Camera;

class FrustumTest : public ::testing::Test {

protected:
    FrustumTest();

    virtual ~FrustumTest();

    // Code here will be called immediately after the constructor (right
    // before each test).
    virtual void SetUp();

    // Code here will be called immediately after each test (right
    // before the destructor).
    virtual void TearDown();

};



FrustumTest::FrustumTest()
{

}

FrustumTest::~FrustumTest()
{

}

void FrustumTest::SetUp()
{
}

void FrustumTest::TearDown()
{
}

TEST_F(FrustumTest, BBVisibilityTest)
{
    Scene scene;
    SceneNode& node = scene.getRoot().addChild(SceneNode());
    node.setLocalBounds(mork::box3d(-1, 1, -1, 1, -1, 1));
    node.setLocalToParent(mat4d::translate(vec3d(10, 0, 0)));
   
    
    scene.getCamera().setPosition(vec3d(-10, 0, 0));
    scene.getCamera().lookAt(vec3d(1,0,0), vec3d(0,0,1));
    scene.getCamera().setFOV(radians(45.0));
    scene.getCamera().setAspectRatio(800, 600);

    scene.update();


    ASSERT_EQ(scene.getCamera().getWorldFrustum().isInside(vec3d(0,0,0)), true);
    ASSERT_EQ(scene.getCamera().getWorldFrustum().getVisibility(node.getWorldBounds()), mork::Visibility::FULLY_VISIBLE);

    // Put the node in the intersection of the upper frustum plane:
    node.setLocalToParent(mat4d::translate(vec3d(10.0*cos(radians(45.0)), 0.0, 10.0*sin(radians(45.0)))));
    scene.update();

    ASSERT_EQ(scene.getCamera().getWorldFrustum().getVisibility(node.getWorldBounds()), mork::Visibility::PARTIALLY_VISIBLE);

    // Put the node above the intersection of the upper frustum plane:
    // Should be invisible
    node.setLocalToParent(mat4d::translate(vec3d(10.0*cos(radians(45.0)), 0.0, 10.0*sin(radians(45.0)) + 2.5)));
    scene.update();

    ASSERT_EQ(scene.getCamera().getWorldFrustum().getVisibility(node.getWorldBounds()), mork::Visibility::INVISIBLE);



    // Put the node behind us
    node.setLocalToParent(mat4d::translate(vec3d(-20.0, 0.0, 0.0)));
    scene.update();

    ASSERT_EQ(scene.getCamera().getWorldFrustum().getVisibility(node.getWorldBounds()), mork::Visibility::INVISIBLE);
    ASSERT_EQ(scene.getCamera().getWorldFrustum().isInside(vec3d(-20,0.0,0.0)), false);

    // Put the node in the intersection of the right frustum plane:
    node.setLocalToParent(mat4d::translate(
                vec3d(
                    10.0*cos(radians(45.0*scene.getCamera().getAspectRatio())),
                    -10.0*sin(radians(45.0*scene.getCamera().getAspectRatio())),
                            0.0)));
    scene.update();

    ASSERT_EQ(scene.getCamera().getWorldFrustum().getVisibility(node.getWorldBounds()), mork::Visibility::PARTIALLY_VISIBLE);

    // Put the node in the intersection of the left frustum plane:
    node.setLocalToParent(mat4d::translate(
                vec3d(
                    10.0*cos(radians(45.0*scene.getCamera().getAspectRatio())),
                    10.0*sin(radians(45.0*scene.getCamera().getAspectRatio())),
                            0.0)));
    scene.update();

    ASSERT_EQ(scene.getCamera().getWorldFrustum().getVisibility(node.getWorldBounds()), mork::Visibility::PARTIALLY_VISIBLE);

    // Put the node in the intersection of the lower frustum plane:
    node.setLocalToParent(mat4d::translate(vec3d(10.0*cos(radians(45.0)), 0.0, -10.0*sin(radians(45.0)))));
    scene.update();

    ASSERT_EQ(scene.getCamera().getWorldFrustum().getVisibility(node.getWorldBounds()), mork::Visibility::PARTIALLY_VISIBLE);

    // Put the node below the intersection of the upper frustum plane:
    // Should be invisible
    node.setLocalToParent(mat4d::translate(vec3d(10.0*cos(radians(45.0)), 0.0, -10.0*sin(radians(45.0)) - 2.5)));
    scene.update();

    ASSERT_EQ(scene.getCamera().getWorldFrustum().getVisibility(node.getWorldBounds()), mork::Visibility::INVISIBLE);



}
    

TEST_F(FrustumTest, PointVisibilityTest)
{
    Scene scene;
    
    scene.getCamera().setPosition(vec3d(0, 0, 0));
    scene.getCamera().lookAt(vec3d(1,0,0), vec3d(0,0,1));
    scene.getCamera().setFOV(radians(45.0));
    scene.getCamera().setAspectRatio(800, 600);

    scene.update();

    ASSERT_EQ(scene.getCamera().getWorldFrustum().isInside(vec3d(1,0,0)), true);
    ASSERT_EQ(scene.getCamera().getWorldFrustum().isInside(vec3d(0,0,0)), false);
    ASSERT_EQ(scene.getCamera().getWorldFrustum().isInside(vec3d(10,0,0)), true);
    ASSERT_EQ(scene.getCamera().getWorldFrustum().isInside(vec3d(0,1,0)), false);
    ASSERT_EQ(scene.getCamera().getWorldFrustum().isInside(vec3d(0,-1,0)), false);
    ASSERT_EQ(scene.getCamera().getWorldFrustum().isInside(vec3d(0,0,1)), false);
    ASSERT_EQ(scene.getCamera().getWorldFrustum().isInside(vec3d(0,0,-1)), false);

    // Test clipping planes (default is 0.1 - 100, but frustum dont test for far plane)
    ASSERT_EQ(scene.getCamera().getWorldFrustum().isInside(vec3d(0.09,0,0)), false);
    ASSERT_EQ(scene.getCamera().getWorldFrustum().isInside(vec3d(0.11,0,0)), true);
    ASSERT_EQ(scene.getCamera().getWorldFrustum().isInside(vec3d(10,0,0)), true);
    ASSERT_EQ(scene.getCamera().getWorldFrustum().isInside(vec3d(99,0,0)), true);
    ASSERT_EQ(scene.getCamera().getWorldFrustum().isInside(vec3d(101,0,0)), true);
    ASSERT_EQ(scene.getCamera().getWorldFrustum().isInside(vec3d(1.0e6,0,0)), true);














}

