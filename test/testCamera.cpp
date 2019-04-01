#include "../mork/scene/Camera.h"
#include "../mork/scene/Scene.h"
#include "../mork/scene/SceneNode.h"
#include "../mork/math/mat4.h"



#include <gtest/gtest.h>

using mork::SceneNode;
using mork::mat4d;
using mork::vec3d;
using mork::vec4d;
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

TEST_F(CameraTest, PropertiesTest)
{

    Camera c1;

    ASSERT_EQ(c1.getFarClippingPlane(), 100.0);
    ASSERT_EQ(c1.getNearClippingPlane(), 0.1);
    ASSERT_EQ(c1.getAspectRatio(), 800.0/600.0);
    ASSERT_EQ(c1.getFOV(), radians(45.0));

    c1.setFOV(radians(30.0));
    ASSERT_EQ(c1.getFOV(), radians(30.0));

    c1.setAspectRatio(700.0, 600.0);
    ASSERT_EQ(c1.getAspectRatio(), 700.0/600.0);

    c1.setClippingPlanes(0.2, 200.0);
    ASSERT_EQ(c1.getFarClippingPlane(), 200.0);
    ASSERT_EQ(c1.getNearClippingPlane(), 0.2);

  

    mork::mat4d proj = mork::mat4d::perspectiveProjection(radians(30.0), 700.0/600.0, 0.2, 200);
    ASSERT_EQ(proj, c1.getProjectionMatrix()); 
 


}


TEST_F(CameraTest, AbsPosTest)
{
    Scene scene;

    SceneNode& object = scene.getRoot().addChild(SceneNode());

    object.setLocalToParent(mat4d::translate(vec3d(10, 0, 0)));


    scene.getCamera().setPosition(vec3d(-10, 0, 0));
    scene.getCamera().lookAt(vec3d(1,0,0), vec3d(0, 0, 1));

  

    scene.update();

    mat4d model = object.getLocalToWorld();
    mat4d view = scene.getCamera().getViewMatrix();

    ASSERT_LT((view * model * vec4d(0, 0, 0, 1) - vec4d(0, 0, -20, 0)).xyz().length(), 1.0E-6);

    
}

/*
TEST_F(CameraTest, ReferenceTest)
{
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();;
    SceneNode object;
    
    ASSERT_NE(&scene.getCamera().getReference(), nullptr);

    scene.getCamera().setReference(object);
    ASSERT_EQ(&scene.getCamera().getReference(), &object);



}
 */
TEST_F(CameraTest, RelPosTest)
{
    Scene scene;

    SceneNode& object = scene.getRoot().addChild(SceneNode());

    object.setLocalToParent(mat4d::translate(vec3d(10, 0, 0)));

    //std::cout << object->getLocalToParent() << std::endl;

    scene.getCamera().setPosition(vec3d(-20, 0, 0));
    scene.getCamera().lookAt(vec3d(1,0,0), vec3d(0, 0, 1));
    scene.getCamera().setReference(object);
    //std::cout << scene.getCamera().getLocalToParent() << std::endl;


    scene.update();

    mat4d model = object.getLocalToWorld();
    mat4d view = scene.getCamera().getViewMatrix();

    ASSERT_LT((view * model * vec4d(0, 0, 0, 1) - vec4d(0, 0, -20, 0)).xyz().length(), 1.0E-6);

    
}

TEST_F(CameraTest, RelPosTest2)
{
    Scene scene;

    SceneNode& object = scene.getRoot().addChild(SceneNode("1"));

    object.setLocalToParent(mat4d::rotatez(radians(90.0))*mat4d::translate(vec3d(10, 0, 0)));

    //std::cout << object->getLocalToParent() << std::endl;

    scene.getCamera().setReference(object);
    
    // These are now in objects frame of reference
    scene.getCamera().setPosition(vec3d(-20, 0, 0));
    scene.getCamera().lookAt(vec3d(1,0,0), vec3d(0, 0, 1));
    //std::cout << scene.getCamera().getLocalToParent() << std::endl;

    
    SceneNode& object2 =  scene.getRoot().addChild(SceneNode("2"));
    
    object2.setLocalToParent(mat4d::translate(vec3d(0, -5, 0)));

    scene.update();

    mat4d model = object.getLocalToWorld();
    mat4d model2 = object2.getLocalToWorld();
    mat4d view = scene.getCamera().getViewMatrix();

    //std::cout << view*model*vec4d(0,0,0,1) << std::endl;
    //std::cout << view*model2*vec4d(0,0,0,1) << std::endl;
    //std::cout << object2->getLocalToWorld() << std::endl;
    //std::cout << scene.getCamera().getViewMatrix()*vec4d(0,0,0,1) << std::endl;
    ASSERT_LT((view * model * vec4d(0, 0, 0, 1) - vec4d(0, 0, -20, 0)).xyz().length(), 1.0E-6);
    ASSERT_LT((view * model2 * vec4d(0, 0, 0, 1) - vec4d(0, 0, -5, 0)).xyz().length(), 1.0E-6);

    


   
}

// test direction of a camera which is absolute (not attached to an object)
TEST_F(CameraTest, DirectionTest)
{
    Scene scene;

    scene.getCamera().setPosition(vec3d(0, 0, 0));
    scene.getCamera().lookAt(vec3d(1,0,0), vec3d(0, 0, 1));
    scene.update();
    ASSERT_LT((scene.getCamera().getWorldPos() - mork::vec3d(0, 0, 0)).length(), 1.0E-6);
    ASSERT_LT((vec3d(1, 0, 0) - scene.getCamera().getWorldForward()).length(), 1.0E-6);
    ASSERT_LT((vec3d(0, 0, 1) - scene.getCamera().getWorldUp()).length(), 1.0E-6);
    ASSERT_LT((vec3d(0, -1, 0) - scene.getCamera().getWorldRight()).length(), 1.0E-6);



    scene.getCamera().lookAt(vec3d(-1,0,0), vec3d(0, 0, 1));
    scene.update();
    ASSERT_LT((vec3d(-1, 0, 0) - scene.getCamera().getWorldForward()).length(), 1.0E-6);
    ASSERT_LT((vec3d(0, 0, 1) - scene.getCamera().getWorldUp()).length(), 1.0E-6);
    ASSERT_LT((vec3d(0, 1, 0) - scene.getCamera().getWorldRight()).length(), 1.0E-6);


    scene.getCamera().lookAt(vec3d(0,1,0), vec3d(0, 0, 1));
    scene.update();
    ASSERT_LT((vec3d(0, 1, 0) - scene.getCamera().getWorldForward()).length(), 1.0E-6);
    ASSERT_LT((vec3d(0, 0, 1) - scene.getCamera().getWorldUp()).length(), 1.0E-6);
    ASSERT_LT((vec3d(1, 0, 0) - scene.getCamera().getWorldRight()).length(), 1.0E-6);


    scene.getCamera().lookAt(vec3d(0,-1,0), vec3d(0, 0, 1));
    scene.update();
    ASSERT_LT((vec3d(0, -1, 0) - scene.getCamera().getWorldForward()).length(), 1.0E-6);
    ASSERT_LT((vec3d(0, 0, 1) - scene.getCamera().getWorldUp()).length(), 1.0E-6);
    ASSERT_LT((vec3d(-1, 0, 0) - scene.getCamera().getWorldRight()).length(), 1.0E-6);


    scene.getCamera().lookAt(vec3d(0,0,1), vec3d(-1, 0, 0));
    scene.update();
    ASSERT_LT((vec3d(0, 0, 1) - scene.getCamera().getWorldForward()).length(), 1.0E-6);
    ASSERT_LT((vec3d(-1, 0, 0) - scene.getCamera().getWorldUp()).length(), 1.0E-6);
    ASSERT_LT((vec3d(0, -1, 0) - scene.getCamera().getWorldRight()).length(), 1.0E-6);


    scene.getCamera().lookAt(vec3d(0,0,-1), vec3d(-1, 0, 0));
    scene.update();
    ASSERT_LT((vec3d(0, 0, -1) - scene.getCamera().getWorldForward()).length(), 1.0E-6);
    ASSERT_LT((vec3d(-1, 0, 0) - scene.getCamera().getWorldUp()).length(), 1.0E-6);
    ASSERT_LT((vec3d(0, 1, 0) - scene.getCamera().getWorldRight()).length(), 1.0E-6);



}


// Test direction of a camera attached to an object:
// Lookat now refers to ojects reference frame
TEST_F(CameraTest, DirectionTest2)
{
    Scene scene;

    SceneNode& object = scene.getRoot().addChild(SceneNode());
    scene.getCamera().setReference(object);
    object.setLocalToParent(mat4d::rotatez(radians(90.0))*mat4d::translate(mork::vec3d(100, 0, 0)));


    scene.getCamera().setPosition(vec3d(0, 0, 0));
    scene.getCamera().lookAt(vec3d(1,0,0), vec3d(0, 0, 1));
    scene.update();
    ASSERT_LT((scene.getCamera().getWorldPos() - mork::vec3d(0,100, 0)).length(), 1.0E-6);
    ASSERT_LT((vec3d(0, 1, 0) - scene.getCamera().getWorldForward()).length(), 1.0E-6);

    scene.getCamera().lookAt(vec3d(-1,0,0), vec3d(0, 0, 1));
    scene.update();
    ASSERT_LT((vec3d(0, -1, 0) - scene.getCamera().getWorldForward()).length(), 1.0E-6);

    scene.getCamera().lookAt(vec3d(0,1,0), vec3d(0, 0, 1));
    scene.update();
    ASSERT_LT((vec3d(-1, 0, 0) - scene.getCamera().getWorldForward()).length(), 1.0E-6);

    scene.getCamera().lookAt(vec3d(0,-1,0), vec3d(0, 0, 1));
    scene.update();
    ASSERT_LT((vec3d(1, 0, 0) - scene.getCamera().getWorldForward()).length(), 1.0E-6);

    scene.getCamera().lookAt(vec3d(0,0,1), vec3d(-1, 0, 0));
    scene.update();
    ASSERT_LT((vec3d(0, 0, 1) - scene.getCamera().getWorldForward()).length(), 1.0E-6);

    scene.getCamera().lookAt(vec3d(0,0,-1), vec3d(-1, 0, 0));
    scene.update();
    ASSERT_LT((vec3d(0, 0, -1) - scene.getCamera().getWorldForward()).length(), 1.0E-6);


}



