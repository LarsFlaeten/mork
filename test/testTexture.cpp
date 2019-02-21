#include "../mork/render/Texture.cpp"

#include <gtest/gtest.h>



class TextureTest : public ::testing::Test {

protected:
    TextureTest();

    virtual ~TextureTest();

    // Code here will be called immediately after the constructor (right
    // before each test).
    virtual void SetUp();

    // Code here will be called immediately after each test (right
    // before the destructor).
    virtual void TearDown();

    mork::GlfwWindow window;
};



TextureTest::TextureTest()
    : window(mork::Window::Parameters().size(800,600))
{

}

TextureTest::~TextureTest()
{

}

void TextureTest::SetUp()
{
}

void TextureTest::TearDown()
{
}

TEST_F(TextureTest, LoadTexture2dTest1)
{
    mork::Texture<2> tex2d;
    tex2d.loadTexture("textures/container.jpg", false);
    ASSERT_EQ(tex2d.getDepth(), 1);    
    ASSERT_EQ(tex2d.getWidth(), 512);
    ASSERT_EQ(tex2d.getHeight(), 512);
    ASSERT_EQ(tex2d.getFormat(), GL_RGB8);

    tex2d.loadTexture("textures/awesomeface.png", true);
    ASSERT_EQ(tex2d.getDepth(), 1);    
    ASSERT_EQ(tex2d.getWidth(), 512);
    ASSERT_EQ(tex2d.getHeight(), 512);
    ASSERT_EQ(tex2d.getFormat(), GL_RGBA8);

    tex2d.loadTexture("textures/container2_specular.png", true);
    ASSERT_EQ(tex2d.getDepth(), 1);    
    ASSERT_EQ(tex2d.getWidth(), 500);
    ASSERT_EQ(tex2d.getHeight(), 500);
    ASSERT_EQ(tex2d.getFormat(), GL_RGBA8);



}

TEST_F(TextureTest, Texture2dEqualityTest)
{
    mork::Texture<2> tex2d1, tex2d2;
    tex2d1.loadTexture("textures/container.jpg", false);

    tex2d2.loadTexture("textures/awesomeface.png", true);
    
    
    ASSERT_EQ(tex2d1 == tex2d2, false);
    ASSERT_EQ(tex2d1 != tex2d2, true);
    ASSERT_EQ(tex2d1 == tex2d1, true);
    ASSERT_EQ(tex2d1 != tex2d1, false);
    ASSERT_EQ(tex2d2 == tex2d2, true);
    ASSERT_EQ(tex2d2 != tex2d2, false);









}

TEST_F(TextureTest, Texture2dMoveAndVector)
{
    mork::Texture<2> tex2d1, tex2d2;
    tex2d1.loadTexture("textures/container.jpg", false);

    tex2d2.loadTexture("textures/awesomeface.png", true);
    
    
    std::vector<mork::Texture<2> > textures;

    // Standard push bu move
    textures.push_back(std::move(tex2d1));

    // Construct in place
    textures.push_back(mork::Texture<2>());
    

    




}


