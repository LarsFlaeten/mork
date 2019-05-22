#include "../mork/render/Program.cpp"

#include <gtest/gtest.h>



class ProgramTest : public ::testing::Test {

protected:
    ProgramTest();

    virtual ~ProgramTest();

    // Code here will be called immediately after the constructor (right
    // before each test).
    virtual void SetUp();

    // Code here will be called immediately after each test (right
    // before the destructor).
    virtual void TearDown();

    mork::GlfwWindow window;
};



ProgramTest::ProgramTest()
    : window(mork::Window::Parameters().size(800,600))
{

}

ProgramTest::~ProgramTest()
{

}

void ProgramTest::SetUp()
{
}

void ProgramTest::TearDown()
{
}

TEST_F(ProgramTest, PreprocessShaderTest1)
{
    std::string test = "#version 330 core\n2\n3\n4";

    std::string string2 = mork::Shader::preProcess(test);

    //count lines:
    std::stringstream ss(string2);
    std::string line;
    int C = 0;
    while(std::getline(ss, line)) { 
        C++;
    } 
    ASSERT_EQ(C, 4);
}


TEST_F(ProgramTest, PreprocessShaderTest2)
{
    std::string test = "#version 330 core\n#include \"shaders/lights.glhl\"\n";

    std::string string2 = mork::Shader::preProcess(test);

    //count lines:
    std::stringstream ss(string2);
    std::string line;
    int C = 0;
    while(std::getline(ss, line)) { 
        C++;
    } 
    ASSERT_GT(C, 2);
}

TEST_F(ProgramTest, PreprocessShaderTest3)
{
    std::string test = "#version 330 core\n#include \"NOT_AN_EXISTING_FILE\"\n";

    ASSERT_THROW(std::string string2 = mork::Shader::preProcess(test),
            std::runtime_error);

}

TEST_F(ProgramTest, PreprocessShaderTest4)
{
    std::ofstream of("/tmp/test_include.glhl");
    of << "Line 1\n" << "Line 2\n\nLine4\n";
    of.close();


    std::string test = "#version 330 core\n#include \"/tmp/test_include.glhl\"\nLast Line\n";

    std::string string2 = mork::Shader::preProcess(test);

    //count lines:
    std::stringstream ss(string2);
    std::string line;
    int C = 0;
    while(std::getline(ss, line)) { 
        C++;
        //std::cout << C << " " << line << std::endl;
    } 
    ASSERT_EQ(C, 6);
}

TEST_F(ProgramTest, ShaderFromFileTestErrors)
{
    std::ofstream of("/tmp/test.glsl");
    of << "Line 1\n" << "Line 2\n\nLine4\n";
    of.close();

    // SHould throw since no _VERTEX_ or _FRAGMENT_
    ASSERT_THROW(mork::Program prog(330, "/tmp/test.glsl"), std::runtime_error);
    
    // SHould throw due to file not found
    ASSERT_THROW(mork::Program prog(330, "no_such_file.glsl"), std::runtime_error);
    
    // Should throw due ot only _VERTEX_, no _FRAGMENT
    std::ofstream of2("/tmp/test.glsl");
    of2 << "Line 1\n" << "#ifdef _VERTEX_\nLine 2\n#endif\nLine4\n";
    of2.close();
    ASSERT_THROW(mork::Program prog(330, "/tmp/test.glsl"), std::runtime_error);
 
    // Should throw due ot only _FRAGMENT_, no _VERTEX
    std::ofstream of3("/tmp/test.glsl");
    of3 << "Line 1\n" << "#ifdef _FRAGMENT_\nLine 2\n#endif\nLine4\n";
    of3.close();
    ASSERT_THROW(mork::Program prog(330, "/tmp/test.glsl"), std::runtime_error);
 
}

TEST_F(ProgramTest, ShaderFromFileVSGSFS)
{
    mork::Program prog(330, "shaders/normalShader.glsl"); 

}
