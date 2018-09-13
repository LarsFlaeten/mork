#include "mork/glad/glad.h"
#include "mork/render/Program.h"
#include "mork/core/Log.h"

#include <cstring>

namespace mork {



Program::Program(const std::string& vssrc, const std::string& fssrc)
 : _programId(0)
{
    const char* c_vs = vssrc.c_str();
    const char* c_fs = fssrc.c_str();
    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &c_vs, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        mork::error_logger("SHADER::VERTEX::COMPILATION_FAILED: ", infoLog); 
        throw std::runtime_error(infoLog);
    }
    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &c_fs, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        mork::error_logger("SHADER::FRAGMENT::COMPILATION_FAILED: ", infoLog);
        throw std::runtime_error(infoLog);
    }
    // link shaders
    _programId = glCreateProgram();
    glAttachShader(_programId, vertexShader);
    glAttachShader(_programId, fragmentShader);
    glLinkProgram(_programId);
    // check for linking errors
    glGetProgramiv(_programId, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(_programId, 512, NULL, infoLog);
        mork::error_logger("SHADER::PROGRAM::LINKING_FAILED: ", infoLog);
        throw std::runtime_error(infoLog);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);




}

Program::~Program() 
{

// This segfaults when glfw shuts down...
// TODO: Find out why and Fix
//    if(_programId)
//        glDeleteProgram(_programId);
 
}

void    Program::use() const
{
    if(!_programId)
        throw std::runtime_error("Program was not cerated before attempting to use it");
    glUseProgram(_programId);
 
}

int Program::getProgramID() const
{
    return _programId;
}



}
