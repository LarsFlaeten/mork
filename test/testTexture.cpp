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
    ASSERT_EQ(tex2d.getNumChannels(), 3);

    tex2d.loadTexture("textures/awesomeface.png", true);
    ASSERT_EQ(tex2d.getDepth(), 1);    
    ASSERT_EQ(tex2d.getWidth(), 512);
    ASSERT_EQ(tex2d.getHeight(), 512);
    ASSERT_EQ(tex2d.getNumChannels(), 4);



}


