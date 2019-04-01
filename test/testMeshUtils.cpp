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

    using Vert = mork::vertex_pos_norm_uv;

    mork::Mesh<Vert> mesh(mork::MeshHelper<Vert>::BOX());
    ASSERT_EQ(mesh.getNumVertices(), 36);
    ASSERT_EQ(mesh.getNumIndices(), 36);
    ASSERT_EQ(mesh.isIndexed(), true);

    mork::Mesh<Vert> mesh2(mork::MeshHelper<Vert>::PLANE());
    ASSERT_EQ(mesh2.getNumVertices(), 4);
    ASSERT_EQ(mesh2.getNumIndices(), 6);
    ASSERT_EQ(mesh2.isIndexed(), true);

    auto t_mesh = mork::MeshUtil::calculateTBNMesh(mesh2);

}


