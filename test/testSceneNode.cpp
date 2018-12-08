#include "../mork/scene/SceneNode.h"
#include "../mork/scene/Scene.h"
#include "../mork/math/mat4.h"


#include <gtest/gtest.h>

using mork::SceneNode;
using mork::mat4d;
using mork::vec3d;
using mork::vec4d;
using mork::Scene;

class SceneNodeTest : public ::testing::Test {

protected:
    SceneNodeTest();

    virtual ~SceneNodeTest();

    // Code here will be called immediately after the constructor (right
    // before each test).
    virtual void SetUp();

    // Code here will be called immediately after each test (right
    // before the destructor).
    virtual void TearDown();

};



SceneNodeTest::SceneNodeTest()
{

}

SceneNodeTest::~SceneNodeTest()
{

}

void SceneNodeTest::SetUp()
{
}

void SceneNodeTest::TearDown()
{
}

TEST_F(SceneNodeTest, InitTest)
{
    SceneNode root;
    root.setLocalToParent(mat4d::translate(vec3d(2,3,4)));
    root.updateLocalToWorld(mat4d::IDENTITY);

    mat4d localToParent = root.getLocalToParent();
    mat4d localToWorld = root.getLocalToWorld();    
    
    ASSERT_EQ(localToParent, mat4d::translate(vec3d(2,3,4)));
    ASSERT_EQ(localToWorld, localToParent);
}

TEST_F(SceneNodeTest, WorldLocal)
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
    ASSERT_EQ(pos, object->getWorldPos());
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


TEST_F(SceneNodeTest, WorldLocal2)
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

TEST_F(SceneNodeTest, WorldLocal3)
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

TEST_F(SceneNodeTest, WorldLocal4)
{
    Scene scene;
    
    
    
    std::shared_ptr<SceneNode> object = std::make_shared<SceneNode>();
    object->setLocalToParent(mat4d::translate(vec3d(0,10,0))*mat4d::rotatez(radians(90.0)));
    scene.getRoot().addChild(object);

    std::shared_ptr<SceneNode> object2 = std::make_shared<SceneNode>();
    object2->setLocalToParent(mat4d::rotatez(radians(90.0))*mat4d::translate(vec3d(10,0,0)));
    scene.getRoot().addChild(object2);



    
    scene.update();


    ASSERT_LT((object->getLocalToWorld()*vec4d(0,0,0,1) - object2->getLocalToWorld()*vec4d(0,0,0,1)).xyz().length(), 1.0E-10);
}

TEST_F(SceneNodeTest, Bounds1)
{
    Scene scene;
    
    
    
    std::shared_ptr<SceneNode> object = std::make_shared<SceneNode>();
    object->setLocalToParent(mat4d::translate(vec3d(10,0,0)));
    object->setLocalBounds(mork::box3d(-1, 1, -1, 1, -1, 1));
    scene.getRoot().addChild(object);

    
    scene.update();

    // Root node should now hav an expanded bounds box which includes objects bounds box
    mork::box3d bb = scene.getRoot().getWorldBounds();
    mork::box3d co = mork::box3d(0, 11, -1, 1, -1, 1);
    ASSERT_EQ(bb.xmax, co.xmax);
    ASSERT_EQ(bb.xmin, co.xmin);
    ASSERT_EQ(bb.ymax, co.ymax);
    ASSERT_EQ(bb.ymin, co.ymin);
    ASSERT_EQ(bb.zmax, co.zmax);
    ASSERT_EQ(bb.zmin, co.zmin);
 
    
}

TEST_F(SceneNodeTest, Bounds2)
{
    Scene scene;
    
    
    
    std::shared_ptr<SceneNode> object = std::make_shared<SceneNode>();
    object->setLocalToParent(mat4d::translate(vec3d(0, 10,0))*mat4d::rotatez(radians(90.0)));
    object->setLocalBounds(mork::box3d(-1, 1, -1, 1, -1, 1));
    scene.getRoot().addChild(object);

    std::shared_ptr<SceneNode> object2 = std::make_shared<SceneNode>();
    object2->setLocalToParent(mat4d::rotatez(radians(90.0))*mat4d::translate(vec3d(10, 0,0)));
    object2->setLocalBounds(mork::box3d(-1, 1, -1, 1, -1, 1));
    scene.getRoot().addChild(object2);

   
    scene.update();

    // Root node should now hav an expanded bounds box which includes objects bounds box
    mork::box3d bb = scene.getRoot().getWorldBounds();
    mork::box3d co = mork::box3d(-1, 1, 0, 11, -1, 1);
    ASSERT_LT(fabs(bb.xmax- co.xmax), 1.0E-12);
    ASSERT_LT(fabs(bb.xmin- co.xmin), 1.0E-12);
    ASSERT_LT(fabs(bb.ymax- co.ymax), 1.0E-12);
    ASSERT_LT(fabs(bb.ymin- co.ymin), 1.0E-12);
    ASSERT_LT(fabs(bb.zmax- co.zmax), 1.0E-12);
    ASSERT_LT(fabs(bb.zmin- co.zmin), 1.0E-12);
 
    
}


TEST_F(SceneNodeTest, Bounds3)
{
    Scene scene;
    
    
    
    std::shared_ptr<SceneNode> object = std::make_shared<SceneNode>();
    object->setLocalToParent(mat4d::translate(vec3d(0, -10,5))*mat4d::rotatez(radians(90.0)));
    object->setLocalBounds(mork::box3d(-10, 1, -1, 1, -1, 2));
    scene.getRoot().addChild(object);

    std::shared_ptr<SceneNode> object2 = std::make_shared<SceneNode>();
    object2->setLocalToParent(mat4d::rotatez(radians(90.0))*mat4d::translate(vec3d(10, 0,0)));
    object2->setLocalBounds(mork::box3d(-1, 1, -1, 1, -1, 1));
    scene.getRoot().addChild(object2);

   
    scene.update();

    // Root node should now hav an expanded bounds box which includes objects bounds box
    mork::box3d bb = scene.getRoot().getWorldBounds();
    mork::box3d co = mork::box3d(-1, 1, -20, 11, -1, 7);
    ASSERT_LT(fabs(bb.xmax- co.xmax), 1.0E-12);
    ASSERT_LT(fabs(bb.xmin- co.xmin), 1.0E-12);
    ASSERT_LT(fabs(bb.ymax- co.ymax), 1.0E-12);
    ASSERT_LT(fabs(bb.ymin- co.ymin), 1.0E-12);
    ASSERT_LT(fabs(bb.zmax- co.zmax), 1.0E-12);
    ASSERT_LT(fabs(bb.zmin- co.zmin), 1.0E-12);
 
    
}

TEST_F(SceneNodeTest, Bounds4)
{
    Scene scene;
    
    
    
    std::shared_ptr<SceneNode> object = std::make_shared<SceneNode>();
    object->setLocalToParent(mat4d::translate(vec3d(0, -10,5)));
    object->setLocalBounds(mork::box3d(-10, 1, -1, 1, -1, 1));
    scene.getRoot().addChild(object);

    std::shared_ptr<SceneNode> object2 = std::make_shared<SceneNode>();
    object2->setLocalToParent(mat4d::rotatez(radians(90.0))*mat4d::translate(vec3d(10, 0,0)));
    object2->setLocalBounds(mork::box3d(-1, 3, -1, 1, -1, 2));
    object->addChild(object2);

   
    scene.update();

    // Root node should now hav an expanded bounds box which includes objects bounds box
    mork::box3d bb = scene.getRoot().getWorldBounds();
    mork::box3d co = mork::box3d(-10, 1, -11, 3, 0, 7);
    ASSERT_LT(fabs(bb.xmax- co.xmax), 1.0E-12);
    ASSERT_LT(fabs(bb.xmin- co.xmin), 1.0E-12);
    ASSERT_LT(fabs(bb.ymax- co.ymax), 1.0E-12);
    ASSERT_LT(fabs(bb.ymin- co.ymin), 1.0E-12);
    ASSERT_LT(fabs(bb.zmax- co.zmax), 1.0E-12);
    ASSERT_LT(fabs(bb.zmin- co.zmin), 1.0E-12);
 
    
}




TEST_F(SceneNodeTest, Scene01) {

    Scene scene;
    scene.getRoot().setLocalToParent(mat4d::translate(vec3d(2,3,4)));
 
    std::shared_ptr<SceneNode> box = std::make_shared<SceneNode>();
    box->setLocalToParent(mat4d::translate(vec3d(-2,-3,-3)));

    scene.getRoot().addChild(box);


    scene.update();
    ASSERT_EQ(box->getLocalToWorld(), mat4d::translate(vec3d(0, 0, 1)));

    
}

TEST_F(SceneNodeTest, Scene02_sharedPtr_benchmark) {

    Scene scene;
    scene.getRoot().setLocalToParent(mat4d::translate(vec3d(2,3,4)));

    for(int i = 0; i < 1000000; ++i) { 
        std::shared_ptr<SceneNode> box = std::make_shared<SceneNode>();
        box->setLocalToParent(mat4d::translate(vec3d(-2,-3,-3)));

        scene.getRoot().addChild(box);
    }


    
}

TEST_F(SceneNodeTest, Scene03_reference_stdmove_benchmark) {

    Scene scene;
    scene.getRoot().setLocalToParent(mat4d::translate(vec3d(2,3,4)));

    for(int i = 0; i < 1000000; ++i) { 
        SceneNode box;
        box.setLocalToParent(mat4d::translate(vec3d(-2,-3,-3)));

        scene.getRoot().addChild(std::move(box));
    }


    
}



