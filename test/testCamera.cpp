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

    std::shared_ptr<SceneNode> object = std::make_shared<SceneNode>();
    scene.getRoot().addChild(object);

    object->setLocalToParent(mat4d::translate(vec3d(10, 0, 0)));

    //std::cout << object->getLocalToParent() << std::endl;

    std::shared_ptr<Camera> camera = std::make_shared<Camera>();;
    camera->setPosition(vec4d(-10, 0, 0, 1));
    camera->lookAt(vec3d(1,0,0), vec3d(0, 0, 1));
    //std::cout << camera->getLocalToParent() << std::endl;

  
    scene.addCamera(camera);

    scene.update();

    mat4d model = object->getLocalToWorld();
    mat4d view = camera->getViewMatrix();

    ASSERT_LT((view * model * vec4d(0, 0, 0, 1) - vec4d(0, 0, -20, 0)).xyz().length(), 1.0E-6);

    
}

TEST_F(CameraTest, ReferenceTest)
{
    std::shared_ptr<Camera> camera = std::make_shared<Camera>();;
    std::shared_ptr<SceneNode> object = std::make_shared<SceneNode>();
    
    ASSERT_EQ(camera->getReference(), nullptr);

    camera->setReference(object);
    ASSERT_EQ(camera->getReference(), object);



}
 
TEST_F(CameraTest, RelPosTest)
{
    Scene scene;

    std::shared_ptr<SceneNode> object = std::make_shared<SceneNode>();
    scene.getRoot().addChild(object);

    object->setLocalToParent(mat4d::translate(vec3d(10, 0, 0)));

    //std::cout << object->getLocalToParent() << std::endl;

    std::shared_ptr<Camera> camera = std::make_shared<Camera>();;
    camera->setPosition(vec4d(-20, 0, 0, 1));
    camera->lookAt(vec3d(1,0,0), vec3d(0, 0, 1));
    camera->setReference(object);
    //std::cout << camera->getLocalToParent() << std::endl;

  
    scene.addCamera(camera);

    scene.update();

    mat4d model = object->getLocalToWorld();
    mat4d view = camera->getViewMatrix();

    ASSERT_LT((view * model * vec4d(0, 0, 0, 1) - vec4d(0, 0, -20, 0)).xyz().length(), 1.0E-6);

    
}

TEST_F(CameraTest, RelPosTest2)
{
    Scene scene;

    std::shared_ptr<SceneNode> object = std::make_shared<SceneNode>();
    scene.getRoot().addChild(object);

    object->setLocalToParent(mat4d::rotatez(radians(90.0))*mat4d::translate(vec3d(10, 0, 0)));

    //std::cout << object->getLocalToParent() << std::endl;

    std::shared_ptr<Camera> camera = std::make_shared<Camera>();;
    camera->setReference(object);
    
    // These are now in objects frame of reference
    camera->setPosition(vec4d(-20, 0, 0, 1));
    camera->lookAt(vec3d(1,0,0), vec3d(0, 0, 1));
    //std::cout << camera->getLocalToParent() << std::endl;

    
    
    std::shared_ptr<SceneNode> object2 = std::make_shared<SceneNode>();
    object2->setLocalToParent(mat4d::translate(vec3d(0, -5, 0)));
    scene.getRoot().addChild(object2);
 
    scene.addCamera(camera);

    scene.update();

    mat4d model = object->getLocalToWorld();
    mat4d model2 = object2->getLocalToWorld();
    mat4d view = camera->getViewMatrix();

    //std::cout << view*model*vec4d(0,0,0,1) << std::endl;
    //std::cout << view*model2*vec4d(0,0,0,1) << std::endl;
    //std::cout << object2->getLocalToWorld() << std::endl;
    //std::cout << camera->getViewMatrix()*vec4d(0,0,0,1) << std::endl;
    ASSERT_LT((view * model * vec4d(0, 0, 0, 1) - vec4d(0, 0, -20, 0)).xyz().length(), 1.0E-6);
    ASSERT_LT((view * model2 * vec4d(0, 0, 0, 1) - vec4d(0, 0, -5, 0)).xyz().length(), 1.0E-6);

    


   
}




/*
TEST_F(CameraTest, WorldLocal)
{
    Scene scene;
    scene.getRoot().setLocalToParent(mat4d::translate(vec3d(2,3,4)));
    
    
    std::shared_ptr<SceneNode> object = std::make_shared<SceneNode>();
    object->setLocalToParent(mat4d::translate(vec3d(3,4,-5)));
    scene.getRoot().addChild(object);

    
    scene.update();

    vec3d pos(5,7,-1);

    mat4d localToParent = object->getLocalToParent();
    mat4d localToWorld = object->getLocalToWorld();    
    mat4d worldToLocal = object->getWorldToLocal();

    vec3d pos0 = worldToLocal*pos;

    ASSERT_EQ(pos0, vec3d::ZERO);
    ASSERT_EQ(pos, localToWorld.translation());
    ASSERT_EQ(pos, localToWorld*vec3d::ZERO);
    ASSERT_EQ(localToParent.translation(), vec3d(3,4,-5));

    scene.getRoot().setLocalToParent(mat4d::IDENTITY);
    std::shared_ptr<SceneNode> obj2 = std::make_shared<SceneNode>();
    obj2->setLocalToParent(mat4d::translate(vec3d(3,4,-5))*mat4d::rotatez(radians(90.0)));
    scene.getRoot().addChild(obj2);    
    scene.update();


    //std::cout << obj2->getLocalToWorld() << std::endl;

    // Since we are rotated, -4,3,5 in local should take us back to 0,0,0
    //std::cout << obj2->getLocalToWorld()*vec3d(-4,3,5) << std::endl;

    ASSERT_LT((obj2->getLocalToWorld()*vec3d(-4,3,5)).length(), 1.0E-10);
    

    
 
}


TEST_F(CameraTest, WorldLocal2)
{
    Scene scene;
    scene.getRoot().setLocalToParent(mat4d::translate(vec3d(10,0,0))*mat4d::rotatez(radians(180.0)));
    
    
    std::shared_ptr<SceneNode> object = std::make_shared<SceneNode>();
    object->setLocalToParent(mat4d::translate(vec3d(10,0,0)));
    scene.getRoot().addChild(object);

    
    scene.update();


    // Since root is rotated z/180, the position of obj @10,0,0 should take us back to 0,0,0
    ASSERT_LT((object->getLocalToWorld()*vec3d(0,0,0)).length(), 1.0E-10);
}

TEST_F(CameraTest, WorldLocal3)
{
    Scene scene;
    
    
    
    std::shared_ptr<SceneNode> object = std::make_shared<SceneNode>();
    object->setLocalToParent(mat4d::translate(vec3d(10,0,0))*mat4d::rotatez(radians(90.0)));
    scene.getRoot().addChild(object);

    std::shared_ptr<SceneNode> object2 = std::make_shared<SceneNode>();
    object2->setLocalToParent(mat4d::translate(vec3d(0,5,10))*mat4d::rotatey(radians(90.0)));
    scene.getRoot().addChild(object2);



    
    scene.update();


    ASSERT_LT((object->getLocalToWorld()*vec3d(0,10,0)).length(), 1.0E-10);
    ASSERT_LT((object2->getLocalToWorld()*vec3d(10,-5,0)).length(), 1.0E-10);
}


TEST_F(CameraTest, Scene01) {

    Scene scene;
    scene.getRoot().setLocalToParent(mat4d::translate(vec3d(2,3,4)));
 
    std::shared_ptr<SceneNode> box = std::make_shared<SceneNode>();
    box->setLocalToParent(mat4d::translate(vec3d(-2,-3,-3)));

    scene.getRoot().addChild(box);


    scene.update();
    ASSERT_EQ(box->getLocalToWorld(), mat4d::translate(vec3d(0, 0, 1)));

    
}

TEST_F(CameraTest, Scene02_sharedPtr_benchmark) {

    Scene scene;
    scene.getRoot().setLocalToParent(mat4d::translate(vec3d(2,3,4)));

    for(int i = 0; i < 1000000; ++i) { 
        std::shared_ptr<SceneNode> box = std::make_shared<SceneNode>();
        box->setLocalToParent(mat4d::translate(vec3d(-2,-3,-3)));

        scene.getRoot().addChild(box);
    }


    
}

TEST_F(CameraTest, Scene03_reference_stdmove_benchmark) {

    Scene scene;
    scene.getRoot().setLocalToParent(mat4d::translate(vec3d(2,3,4)));

    for(int i = 0; i < 1000000; ++i) { 
        SceneNode box;
        box.setLocalToParent(mat4d::translate(vec3d(-2,-3,-3)));

        scene.getRoot().addChild(std::move(box));
    }


    
}

*/

