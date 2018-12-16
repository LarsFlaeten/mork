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

    std::string string2 = mork::Program::preProcessShader(test);

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

    std::string string2 = mork::Program::preProcessShader(test);

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

    ASSERT_THROW(std::string string2 = mork::Program::preProcessShader(test),
            std::runtime_error);

}

TEST_F(ProgramTest, PreprocessShaderTest4)
{
    std::ofstream of("/tmp/test_include.glhl");
    of << "Line 1\n" << "Line 2\n\nLine4\n";
    of.close();


    std::string test = "#version 330 core\n#include \"/tmp/test_include.glhl\"\nLast Line\n";

    std::string string2 = mork::Program::preProcessShader(test);

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


