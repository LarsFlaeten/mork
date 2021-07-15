#include "../mork/render/GPUBuffer.h"
#include "../mork/math/vec4.h"
#include <gtest/gtest.h>



class BufferTest : public ::testing::Test {

protected:
    BufferTest();

    virtual ~BufferTest();

    // Code here will be called immediately after the constructor (right
    // before each test).
    virtual void SetUp();

    // Code here will be called immediately after each test (right
    // before the destructor).
    virtual void TearDown();

    mork::GlfwWindow window;

};



BufferTest::BufferTest()
:  window(mork::Window::Parameters().size(800,600))

{

}

BufferTest::~BufferTest()
{

}

void BufferTest::SetUp()
{
}

void BufferTest::TearDown()
{
}

TEST_F(BufferTest, CreateAndFillShaderStorageTest)
{
    typedef  mork::GPUBuffer<float, GL_SHADER_STORAGE_BUFFER, GL_DYNAMIC_DRAW> SSBO;
    SSBO buf;

    // Fill with some data:
    std::vector<float> v = {
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 10.0f, 11.0f, 12.0f
    };

    buf.bind();
    buf.setData(v);



    auto bw = mork::ConstBufferView<SSBO>(buf);
    
    const float* fptr = reinterpret_cast<const float*>(bw.get());
 
    ASSERT_EQ(1.0, *(fptr));
    ASSERT_EQ(2.0, *(fptr+1));
    
    ASSERT_EQ(6.0, *(fptr+5));
    ASSERT_EQ(12.0, *(fptr+11));
    
}

