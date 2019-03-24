#include "../mork/render/Mesh.cpp"

#include <gtest/gtest.h>



class MeshTest : public ::testing::Test {

protected:
    MeshTest();

    virtual ~MeshTest();

    // Code here will be called immediately after the constructor (right
    // before each test).
    virtual void SetUp();

    // Code here will be called immediately after each test (right
    // before the destructor).
    virtual void TearDown();

    mork::GlfwWindow window;
};



MeshTest::MeshTest()
    : window(mork::Window::Parameters().size(800,600))
{

}

MeshTest::~MeshTest()
{

}

void MeshTest::SetUp()
{
}

void MeshTest::TearDown()
{
}

TEST_F(MeshTest, StandardObjects)
{
    using V = mork::vertex_pos_norm_uv;
    auto mesh(mork::MeshHelper<V>::BOX());
    ASSERT_EQ(mesh.getNumVertices(), 36);
    ASSERT_EQ(mesh.getNumIndices(), 0);
    ASSERT_EQ(mesh.isIndexed(), false);

    auto mesh2(mork::MeshHelper<V>::PLANE());
    ASSERT_EQ(mesh2.getNumVertices(), 4);
    ASSERT_EQ(mesh2.getNumIndices(), 6);
    ASSERT_EQ(mesh2.isIndexed(), true);


}


