#include "../mork/util/MeshUtil.cpp"

#include <gtest/gtest.h>



class MeshUtilsTest : public ::testing::Test {

protected:
    MeshUtilsTest();

    virtual ~MeshUtilsTest();

    // Code here will be called immediately after the constructor (right
    // before each test).
    virtual void SetUp();

    // Code here will be called immediately after each test (right
    // before the destructor).
    virtual void TearDown();

    mork::GlfwWindow window;
};



MeshUtilsTest::MeshUtilsTest()
    : window(mork::Window::Parameters().size(800,600))
{

}

MeshUtilsTest::~MeshUtilsTest()
{

}

void MeshUtilsTest::SetUp()
{
}

void MeshUtilsTest::TearDown()
{
}

TEST_F(MeshUtilsTest, StandardObjects)
{
    mork::BasicMesh mesh(mork::BasicMeshHelper::BOX());
    ASSERT_EQ(mesh.getNumVertices(), 36);
    ASSERT_EQ(mesh.getNumIndices(), 0);
    ASSERT_EQ(mesh.isIndexed(), false);

    mork::BasicMesh mesh2(mork::BasicMeshHelper::PLANE());
    ASSERT_EQ(mesh2.getNumVertices(), 4);
    ASSERT_EQ(mesh2.getNumIndices(), 6);
    ASSERT_EQ(mesh2.isIndexed(), true);

    auto t_mesh = mork::MeshUtil::calculateTBNMesh(mesh2);

}


