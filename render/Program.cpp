#include "mork/glad/glad.h"
#include "mork/render/Program.h"
#include "mork/core/Log.h"
#include "mork/ui/GlfwWindow.h"
#include <cstring>
#include <vector>
#include <string>

namespace mork {



Program::Program(const std::string& vssrc, const std::string& fssrc)
 : _programID(0), _vs(0), _fs(0)
{
    buildShaders(vssrc, fssrc);
}

Program::~Program() 
{
    // TODO: Change method of establishing active context
    // (do not use GLFWWindow)
    if(GlfwWindow::isContextActive())
    {
        if(_programID)
            glDeleteProgram(_programID);
    }   
}


void Program::buildShaders(const std::string& vssrc, const std::string& fssrc) {
    // This will delete the program if it allready exist, and detach shaders
    // (Thease are allready marked for deletion and will be freed)
    if(_programID!=0) {
        glDeleteProgram(_programID);
        _programID = 0;
        _fs = 0;
        _vs = 0;
    }
    
    const char* c_vs = vssrc.c_str();
    const char* c_fs = fssrc.c_str();
    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    mork::debug_logger("Compiling vertex shader");
    _vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(_vs, 1, &c_vs, NULL);
    glCompileShader(_vs);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(_vs, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(_vs, 512, NULL, infoLog);
        mork::error_logger("SHADER::VERTEX::COMPILATION_FAILED: ", infoLog); 
        throw std::runtime_error(infoLog);
    }
    // fragment shader
    mork::debug_logger("Compiling fragment shader");
    _fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(_fs, 1, &c_fs, NULL);
    glCompileShader(_fs);
    // check for shader compile errors
    glGetShaderiv(_fs, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(_fs, 512, NULL, infoLog);
        mork::error_logger("SHADER::FRAGMENT::COMPILATION_FAILED: ", infoLog);
        throw std::runtime_error(infoLog);
    }
    // link shaders
    _programID = glCreateProgram();
    glAttachShader(_programID, _vs);
    glAttachShader(_programID, _fs);
    glLinkProgram(_programID);
    // check for linking errors
    glGetProgramiv(_programID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(_programID, 512, NULL, infoLog);
        mork::error_logger("SHADER::PROGRAM::LINKING_FAILED: ", infoLog);
        throw std::runtime_error(infoLog);
    }
    glDeleteShader(_vs);
    glDeleteShader(_fs);


    // Establish active non-block uniforms in the program
    uniforms.clear();

    int numUniforms = 0;
    glGetProgramInterfaceiv(_programID, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);
    const GLenum properties[4] = {GL_BLOCK_INDEX, GL_TYPE, GL_NAME_LENGTH, GL_LOCATION};
	
	mork::info_logger("Querying Uniforms in program ", _programID);
    if(numUniforms==0)
        mork::info_logger(" ** No uniforms in this program");
	for(int unif = 0; unif < numUniforms; ++unif)
	{
	    GLint values[4];
	    glGetProgramResourceiv(_programID, GL_UNIFORM, unif, 4, properties, 4, NULL, values);

	    // Skip any uniforms that are in a block.
	    if(values[0] != -1)
		    continue;

	    // Get the name. Must use a std::vector rather than a std::string for C++03 standards issues.
	    // C++11 would let you use a std::string directly.
	    std::vector<char> nameData(values[2]);
	    glGetProgramResourceName(_programID, GL_UNIFORM, unif, nameData.size(), NULL, &nameData[0]);
	    std::string name(nameData.begin(), nameData.end() - 1);
		int type = values[1];
        int location = values[3];
        mork::info_logger("Name: \"", name, "\", type: ", type, ", location: ", location);
        uniforms.insert({name, Uniform(type, location)});
    }


}

void    Program::use() const
{
    if(!_programID)
        throw std::runtime_error("Program was not created before attempting to use it");
    glUseProgram(_programID);
 
}

int Program::getProgramID() const
{
    return _programID;
}

const Uniform& Program::getUniform(const std::string& name) {
    auto entry = uniforms.find(name);
    if(entry==uniforms.end()) {
        mork::error_logger("Tried to acess uniform \"", name, "\" which is not active in program ", _programID);
        throw std::runtime_error("Error acessing Uniform, see log");
    } 
    
    return entry->second;
}
 

}
