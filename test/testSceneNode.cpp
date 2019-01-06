#include "../mork/scene/SceneNode.h"
#include "../mork/scene/Scene.h"
#include "../mork/math/mat4.h"
#include "../mork/core/Log.h"


#include <gtest/gtest.h>

using mork::SceneNode;
using mork::mat4d;
using mork::vec3d;
using mork::vec4d;
using mork::Scene;
using mork::debug_logger;

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

// Just a test on reference wrapper and polymprhism
class A {
public:
    virtual ~A() = default;
    
    virtual int getInt() const {return 1;}
    
    int ma;

};

class B : public A {
public:
    virtual int getInt() const {return 2;}

    int mb;
};

TEST_F(SceneNodeTest, refWrapper) {

    std::vector<std::reference_wrapper<A> > vec;

    A a;
    a.ma = 1;


    B b;
    b.ma = 2;
    b.mb = 3;

    A& aref = a;
    A& bref = b;

    vec.push_back(std::reference_wrapper<A>(a));
    vec.push_back(std::reference_wrapper<A>(b));

    // TEST of fetch and polymorphism
    A& aref2 = vec[0];
    A& tmpref = vec[1];
    auto & bref2 = dynamic_cast<B&>(tmpref);
    ASSERT_EQ(aref2.ma, 1);
    ASSERT_EQ(bref2.ma, 2);
    ASSERT_EQ(bref2.mb, 3);

    A& aref3 = vec[0];
    A& bref3 = vec[1];

    ASSERT_EQ(aref3.getInt(), 1);
    // SHould call B's method, even if bref3 is A&:
    ASSERT_EQ(bref3.getInt(), 2);
 
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


TEST_F(SceneNodeTest, InitTest2)
{
    Scene scene;

    // Initialization by moving object directly
    SceneNode& object = scene.getRoot().addChild(SceneNode());
    object.setLocalToParent(mat4d::translate(vec3d(3,4,-5)));

    debug_logger(object.getName());

    // Initialization by creating unique_ptr in place
    SceneNode& object2 = scene.getRoot().addChild(std::unique_ptr<SceneNode>(new SceneNode()));
    object2.setLocalToParent(mat4d::translate(vec3d(7,8,-9)));
    
    // Initialization by moving unique_ptr form the outside (tmp will be invalid after Root has taken ownership)
    std::unique_ptr<SceneNode> tmp = std::make_unique<SceneNode>();
    SceneNode& object3 = scene.getRoot().addChild(std::move(tmp));
    object2.setLocalToParent(mat4d::translate(vec3d(7,8,-9)));
 


    scene.update();

   
    
}

TEST_F(SceneNodeTest, Children)
{
    SceneNode root("root");

    SceneNode& o1 = root.addChild(SceneNode("1"));

    std::unique_ptr<SceneNode> ptr2 = std::make_unique<SceneNode>("2");
    SceneNode& o2 = root.addChild(std::move(ptr2)); 

    ASSERT_EQ(o1.getName(), "1");
    ASSERT_EQ(o2.getName(), "2");
    ASSERT_EQ(root.getName(), "root");
    ASSERT_EQ(o1.getName(), root.getChild("1").getName());
    ASSERT_EQ(&o1, &(root.getChild("1")));
    ASSERT_EQ(o2.getName(), root.getChild("2").getName());
    ASSERT_EQ(&o2, &(root.getChild("2")));



    std::vector<std::string> list = root.listChildren();
    bool f1 = false, f2 = false;
    for(const auto& s : list) {
        if(s == "1")
            f1 = true;
        if(s == "2")
            f2 = true;
    }
    ASSERT_EQ(f1, true);
    ASSERT_EQ(f1, true);
    ASSERT_EQ(list.size(), 2);







    
}

// Test to see that the adress of an element is persistent with growing children size
TEST_F(SceneNodeTest, ChildrenMillions)
{
    SceneNode root("root");
    
    SceneNode& o1 = root.addChild(SceneNode("first"));


    for(int i = 2; i < 1000000; ++i) {
        SceneNode& o = root.addChild(SceneNode(std::to_string(i))); 
        ASSERT_EQ(&o1, &(root.getChild("first")));
    }
    
}

TEST_F(SceneNodeTest, ChildrenRejectDuplicateName1)
{
    SceneNode root("root");

    SceneNode& o1 = root.addChild(SceneNode("1"));

    mork::info_logger("The following error message is expected:");
    try {
        std::unique_ptr<SceneNode> ptr2 = std::make_unique<SceneNode>("1");
        SceneNode& o2 = root.addChild(std::move(ptr2)); 
    } catch (std::runtime_error& e) {
        ASSERT_EQ(e.what(), std::string("Child name allready existing"));

    } catch (...) {
        FAIL() << "Expected std::runtime_error";
    }
}

TEST_F(SceneNodeTest, ChildrenRejectDuplicateName2)
{
    SceneNode root("root");

    SceneNode& o1 = root.addChild(SceneNode("1"));
    mork::info_logger("The following error message is expected:");
 
    try {
        SceneNode& o2 = root.addChild(SceneNode("1")); 
    } catch (std::runtime_error& e) {
        ASSERT_EQ(e.what(), std::string("Child name allready existing"));

    } catch (...) {
        FAIL() << "Expected std::runtime_error";
    }
}

TEST_F(SceneNodeTest, ChildrenIterate1)
{
    SceneNode root("root");

    for(int i = 1; i <= 10; ++i) {
        root.addChild(SceneNode(std::to_string(i)));
    }

    int i = 1;

    // Demonstres how to iterate over the children of a node. Reference_wrapper
    // behaves a a normal reference here:
    for(const SceneNode& child : root.getChildren() ) {
        ASSERT_EQ(std::to_string(i), child.getName());
        ++i;
    }

}


TEST_F(SceneNodeTest, ChildrenRemoveAndReinsert)
{
    SceneNode root("root");

    for(int i = 1; i <= 5; ++i) {
        root.addChild(SceneNode(std::to_string(i)));
    }

    int i = 1;

    // Demonstres how to iterate over the children of a node. Reference_wrapper
    // behaves a a normal reference here:
    for(const SceneNode& child : root.getChildren() ) {
        ASSERT_EQ(std::to_string(i), child.getName());
        ++i;
    }
/*
    debug_logger("Childs before removal");
    for(const auto& ch : root.listChildren())
        debug_logger("  ", ch);
*/
    ASSERT_EQ(root.getChildren().size(), 5);
    ASSERT_EQ(root.hasChild("1"), true);
    ASSERT_EQ(root.hasChild("2"), true);
    ASSERT_EQ(root.hasChild("3"), true);
    ASSERT_EQ(root.hasChild("4"), true);
    ASSERT_EQ(root.hasChild("5"), true);


    // Try removing a child:
    std::unique_ptr<SceneNode> n = root.removeChild(root.getChild("3"));
  /*  
    debug_logger("Childs after removal");
    for(const auto& ch : root.listChildren())
        debug_logger("  ", ch);
*/

    ASSERT_EQ(root.getChildren().size(), 4);
    ASSERT_EQ(root.hasChild("1"), true);
    ASSERT_EQ(root.hasChild("2"), true);
    ASSERT_EQ(root.hasChild("3"), false);
    ASSERT_EQ(root.hasChild("4"), true);
    ASSERT_EQ(root.hasChild("5"), true);

    // Try inserting n again:
    root.addChild(std::move(n));
  /*  
    debug_logger("Childs after insertion");
    for(const auto& ch : root.listChildren())
        debug_logger("  ", ch);
*/

    ASSERT_EQ(root.getChildren().size(), 5);
    ASSERT_EQ(root.hasChild("1"), true);
    ASSERT_EQ(root.hasChild("2"), true);
    ASSERT_EQ(root.hasChild("3"), true);
    ASSERT_EQ(root.hasChild("4"), true);
    ASSERT_EQ(root.hasChild("5"), true);

    // Try removing all:
    std::unique_ptr<SceneNode> n1 = root.removeChild(root.getChild("1"));
    std::unique_ptr<SceneNode> n2 = root.removeChild(root.getChild("2"));
    std::unique_ptr<SceneNode> n3 = root.removeChild(root.getChild("3"));
    std::unique_ptr<SceneNode> n4 = root.removeChild(root.getChild("4"));
    std::unique_ptr<SceneNode> n5 = root.removeChild(root.getChild("5"));
   
    /*
    debug_logger("Childs after removal");
    for(const auto& ch : root.listChildren())
        debug_logger("  ", ch);
*/

    ASSERT_EQ(root.getChildren().size(), 0);
    ASSERT_EQ(root.hasChild("1"), false);
    ASSERT_EQ(root.hasChild("2"), false);
    ASSERT_EQ(root.hasChild("3"), false);
    ASSERT_EQ(root.hasChild("4"), false);
    ASSERT_EQ(root.hasChild("5"), false);

    // Try inserting all again:
    root.addChild(std::move(n1));
    root.addChild(std::move(n2));
    root.addChild(std::move(n3));
    root.addChild(std::move(n4));
    root.addChild(std::move(n5));
 
    ASSERT_EQ(root.getChildren().size(), 5);
    ASSERT_EQ(root.hasChild("1"), true);
    ASSERT_EQ(root.hasChild("2"), true);
    ASSERT_EQ(root.hasChild("3"), true);
    ASSERT_EQ(root.hasChild("4"), true);
    ASSERT_EQ(root.hasChild("5"), true);





}

TEST_F(SceneNodeTest, Equals)
{
    SceneNode root("root");

    SceneNode& test = root.addChild(SceneNode("test"));

    SceneNode test2("test");
    SceneNode test3("other_name");

    ASSERT_EQ(test, root.getChild("test"));
    ASSERT_EQ(test==root.getChild("test"), true);
    ASSERT_EQ(test!=root.getChild("test"), false);

    ASSERT_NE(test2, root.getChild("test"));
    ASSERT_EQ(test2==root.getChild("test"), false);
    ASSERT_EQ(test2!=root.getChild("test"), true);

    ASSERT_NE(test3, root.getChild("test"));
    ASSERT_EQ(test3==root.getChild("test"), false);
    ASSERT_EQ(test3!=root.getChild("test"), true);





}
 
TEST_F(SceneNodeTest, HasNode1)
{
    SceneNode root("root");

    auto& one = root.addChild(SceneNode("1"));

    SceneNode two("two");

    ASSERT_EQ(root.hasChild(one), true);
    ASSERT_EQ(root.hasChild(two), false);



}
 

TEST_F(SceneNodeTest, HasNode2)
{
    SceneNode root("root");

    auto& one = root.addChild(SceneNode("1"));

    SceneNode two("two");

    ASSERT_EQ(root.hasChild("1"), true);
    ASSERT_EQ(root.hasChild("2"), false);



}
 
TEST_F(SceneNodeTest, HasNode1BenchMark)
{
    /*
    SceneNode root("root");
    int max = 1000000;
 
    debug_logger("Inserting ", max, " nodes..");
    for(int i = 0; i <= max; ++i)
        root.addChild(SceneNode(std::to_string(i)));

    debug_logger("Extacting largest element");
    SceneNode& tes = root.getChild(std::to_string(max));
    */


    // The below was the check for alternative method
    // as the original HasChild(SceneNode&) was O(n). but lookup through the hashmap instead gave O(1)
    /*
    mork::Timer timer;

    timer.start();
    debug_logger("Checking existence 200 times");
    for(int j = 0; j < 200; ++j)
        root.hasChild(tes);
    debug_logger("Time: " , timer.end());;

    timer.start();
    debug_logger("Checking existence 200 times, alternative method");
    for(int j = 0; j < 200; ++j)
        root.hasChild2(tes);
    debug_logger("Time: " , timer.end());;
    */
    // hasChild2 was then renamed hasChild and became the method to use

}
 
TEST_F(SceneNodeTest, WorldLocal)
{
    Scene scene;
    scene.getRoot().setLocalToParent(mat4d::translate(vec3d(2,3,4)));
    
    SceneNode& object = scene.getRoot().addChild(SceneNode());
    object.setLocalToParent(mat4d::translate(vec3d(3,4,-5)));
    
    scene.update();

    vec3d pos(5,7,-1);

    mat4d localToParent = object.getLocalToParent();
    mat4d localToWorld = object.getLocalToWorld();    
    mat4d worldToLocal = object.getWorldToLocal();

    vec3d pos0 = worldToLocal*pos;

    ASSERT_EQ(pos0, vec3d::ZERO);
    ASSERT_EQ(pos, localToWorld.translation());
    ASSERT_EQ(pos, localToWorld*vec3d::ZERO);
    ASSERT_EQ(pos, object.getWorldPos());
    ASSERT_EQ(localToParent.translation(), vec3d(3,4,-5));

    scene.getRoot().setLocalToParent(mat4d::IDENTITY);
    SceneNode& obj2 = scene.getRoot().addChild(SceneNode());    
    obj2.setLocalToParent(mat4d::translate(vec3d(3,4,-5))*mat4d::rotatez(radians(90.0)));
    scene.update();


    //std::cout << obj2->getLocalToWorld() << std::endl;

    // Since we are rotated, -4,3,5 in local should take us back to 0,0,0
    //std::cout << obj2->getLocalToWorld()*vec3d(-4,3,5) << std::endl;

    ASSERT_LT((obj2.getLocalToWorld()*vec3d(-4,3,5)).length(), 1.0E-10);
    

    
 
}


TEST_F(SceneNodeTest, WorldLocal2)
{
    Scene scene;
    scene.getRoot().setLocalToParent(mat4d::translate(vec3d(10,0,0))*mat4d::rotatez(radians(180.0)));
    
    SceneNode& object = scene.getRoot().addChild(SceneNode());
    object.setLocalToParent(mat4d::translate(vec3d(10,0,0)));
    
    scene.update();


    // Since root is rotated z/180, the position of obj @10,0,0 should take us back to 0,0,0
    ASSERT_LT((object.getLocalToWorld()*vec3d(0,0,0)).length(), 1.0E-10);
}

TEST_F(SceneNodeTest, WorldLocal3)
{
    Scene scene;
    
    
    
    SceneNode& object = scene.getRoot().addChild(SceneNode());
    object.setLocalToParent(mat4d::translate(vec3d(10,0,0))*mat4d::rotatez(radians(90.0)));

    SceneNode& object2 = scene.getRoot().addChild(SceneNode());
    object2.setLocalToParent(mat4d::translate(vec3d(0,5,10))*mat4d::rotatey(radians(90.0)));


    
    scene.update();


    ASSERT_LT((object.getLocalToWorld()*vec3d(0,10,0)).length(), 1.0E-10);
    ASSERT_LT((object2.getLocalToWorld()*vec3d(10,-5,0)).length(), 1.0E-10);
}

TEST_F(SceneNodeTest, WorldLocal4)
{
    Scene scene;
    
    
    
    SceneNode& object = scene.getRoot().addChild(SceneNode());
    object.setLocalToParent(mat4d::translate(vec3d(0,10,0))*mat4d::rotatez(radians(90.0)));

    SceneNode& object2 = scene.getRoot().addChild(SceneNode());
    object2.setLocalToParent(mat4d::rotatez(radians(90.0))*mat4d::translate(vec3d(10,0,0)));


    
    scene.update();


    ASSERT_LT((object.getLocalToWorld()*vec4d(0,0,0,1) - object2.getLocalToWorld()*vec4d(0,0,0,1)).xyz().length(), 1.0E-10);
}

TEST_F(SceneNodeTest, Bounds1)
{
    Scene scene;
    
    
    SceneNode& object = scene.getRoot().addChild(SceneNode());
    object.setLocalToParent(mat4d::translate(vec3d(10,0,0)));
    object.setLocalBounds(mork::box3d(-1, 1, -1, 1, -1, 1));

    
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
    
    
    SceneNode& object = scene.getRoot().addChild(SceneNode());
    object.setLocalToParent(mat4d::translate(vec3d(0, 10,0))*mat4d::rotatez(radians(90.0)));
    object.setLocalBounds(mork::box3d(-1, 1, -1, 1, -1, 1));

    SceneNode& object2 = scene.getRoot().addChild(SceneNode());
    object2.setLocalToParent(mat4d::rotatez(radians(90.0))*mat4d::translate(vec3d(10, 0,0)));
    object2.setLocalBounds(mork::box3d(-1, 1, -1, 1, -1, 1));

   
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
    
    
    
    SceneNode& object = scene.getRoot().addChild(SceneNode());
    object.setLocalToParent(mat4d::translate(vec3d(0, -10,5))*mat4d::rotatez(radians(90.0)));
    object.setLocalBounds(mork::box3d(-10, 1, -1, 1, -1, 2));

    SceneNode& object2 = scene.getRoot().addChild(SceneNode());
    object2.setLocalToParent(mat4d::rotatez(radians(90.0))*mat4d::translate(vec3d(10, 0,0)));
    object2.setLocalBounds(mork::box3d(-1, 1, -1, 1, -1, 1));

   
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
    
    
    
    SceneNode& object = scene.getRoot().addChild(SceneNode());
    object.setLocalToParent(mat4d::translate(vec3d(0, -10,5)));
    object.setLocalBounds(mork::box3d(-10, 1, -1, 1, -1, 1));

    SceneNode& object2 = object.addChild(SceneNode());
    object2.setLocalToParent(mat4d::rotatez(radians(90.0))*mat4d::translate(vec3d(10, 0,0)));
    object2.setLocalBounds(mork::box3d(-1, 3, -1, 1, -1, 2));

   
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
 
    SceneNode& box = scene.getRoot().addChild(SceneNode());
    box.setLocalToParent(mat4d::translate(vec3d(-2,-3,-3)));



    scene.update();
    ASSERT_EQ(box.getLocalToWorld(), mat4d::translate(vec3d(0, 0, 1)));

    
}

TEST_F(SceneNodeTest, Scene02_reference_stdmove_benchmark) {

    Scene scene;
    scene.getRoot().setLocalToParent(mat4d::translate(vec3d(2,3,4)));

    for(int i = 0; i < 1000000; ++i) { 
        SceneNode& box = scene.getRoot().addChild(SceneNode());
        box.setLocalToParent(mat4d::translate(vec3d(-2,-3,-3)));
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



