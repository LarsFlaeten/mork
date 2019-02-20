#include "../mork/render/Framebuffer.cpp"

#include <gtest/gtest.h>



class FramebufferTest : public ::testing::Test {

protected:
    FramebufferTest();

    virtual ~FramebufferTest();

    // Code here will be called immediately after the constructor (right
    // before each test).
    virtual void SetUp();

    // Code here will be called immediately after each test (right
    // before the destructor).
    virtual void TearDown();

    mork::GlfwWindow window;
};



FramebufferTest::FramebufferTest()
    : window(mork::Window::Parameters().size(800,600))
{

}

FramebufferTest::~FramebufferTest()
{

}

void FramebufferTest::SetUp()
{
}

void FramebufferTest::TearDown()
{
}

TEST_F(FramebufferTest, GetDefaultFramebuffer)
{
    // Should not throw any exceptions
    auto& fb = mork::Framebuffer::getDefault();

    // Need to do this explicitly here, since GLFWindow does not resize the default framebuffer. If you inherit from GLFWWindow, like in the examples, the overridden reshape(x,y) should resize the default framebuffer.
    fb.setSize(mork::vec2i(window.getWidth(), window.getHeight()));
    
    
    fb.bind();
    
    
    ASSERT_EQ(fb.getSize().x, 800);
    ASSERT_EQ(fb.getSize().y, 600);


    
    fb.unbind();

    
}

TEST_F(FramebufferTest, ClearColor)
{
    // Should not throw any exceptions
    auto& fb = mork::Framebuffer::getDefault();

    
    mork::vec4f color(0.2f, 0.3f, 0.4f, 0.5f);
    fb.setClearColor(color);

    mork::vec4f c2 = fb.getClearColor();

    
    
    ASSERT_EQ(color, c2);


    
    fb.unbind();

    
}

TEST_F(FramebufferTest, RenderBuffer01)
{
    // Should not throw any exceptions
    mork::Framebuffer fb(400, 300);

    fb.bind();

    ASSERT_EQ(fb.getSize().x, 400);
    ASSERT_EQ(fb.getSize().y, 300);


    fb.setSize(mork::vec2i(600, 400));
    ASSERT_EQ(fb.getSize().x, 600);
    ASSERT_EQ(fb.getSize().y, 400);


    mork::Framebuffer::getDefault().bind();

    ASSERT_EQ(fb.getSize().x, 600);
    ASSERT_EQ(fb.getSize().y, 400);


    fb.setSize(mork::vec2i(300, 200));
    ASSERT_EQ(fb.getSize().x, 300);
    ASSERT_EQ(fb.getSize().y, 200);

    fb.bind();

    ASSERT_EQ(fb.getSize().x, 300);
    ASSERT_EQ(fb.getSize().y, 200);


    fb.setSize(mork::vec2i(600, 400));
    ASSERT_EQ(fb.getSize().x, 600);
    ASSERT_EQ(fb.getSize().y, 400);

    auto& def_fb = mork::Framebuffer::getDefault();
    ASSERT_EQ(def_fb.getSize().x, 800);
    ASSERT_EQ(def_fb.getSize().y, 600);



    
}


