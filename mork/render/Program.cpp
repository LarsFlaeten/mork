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

Program::Program(Program&& o) {
    _programID = o._programID;
    o._programID = 0;

}

Program& Program::operator=(Program&& o) {
    if(GlfwWindow::isContextActive())
    {
        if(_programID)
            glDeleteProgram(_programID);
    }   
    
    _programID = o._programID;
    o._programID = 0;  

}

std::string   Program::preProcessShader(const std::string& s) {
    std::stringstream ss(s);

    std::string line;
    std::string processed;
    int i = 0;


    while(std::getline(ss, line)) {
        // Line no;
        ++i;        
        
        if(line.find("#include") != std::string::npos) {
            std::string include = line.substr(8, line.length());

            // Remove hyphens and whitespace
            include.erase(std::remove(include.begin(), include.end(), ' '), include.end());
            include.erase(std::remove(include.begin(), include.end(), '\"'), include.end());
            mork::debug_logger("Ecountered include statement, including file: [", include, "]");
            std::ifstream ifs(include);
            if(!ifs.is_open()) {
                mork::error_logger("Could not open file \"", include, "\".");
                throw std::runtime_error("Error preprocessing shader");

            }
            std::string include_str;
            std::string iline;
            int j = 0;
            while(std::getline(ifs, iline)) {
               ++j;
               include_str.append(iline); 
               include_str.append("\n");
            }
            mork::debug_logger("Read ", j, " lines: ");
            //mork::debug_logger(include_str);

            // trim last endline, since it will be reappended when adde to outer
            if(include_str[include_str.length()-1] == '\n')
                include_str = include_str.substr(0, include_str.length()-1);

            // Recursively preprocess the included shader
            preProcessShader(include_str);

            // replace the include statement with the actual included file:
            line = include_str;
        
        
        }

        processed.append(line);
        processed.append("\n");
    }


    // Check the complete shader:
    // #version should be on first line
    // No version directives later
    /*
        if(i==1 && line.find("#version") != 0){
            mork::warn_logger("Version preprocessor directive not in first line og GLSL, adding");
            processed.append("#version 330 core\n");
        }
*/

    return processed;
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
   
    std::string vs_proc = preProcessShader(vssrc);
    std::string fs_proc = preProcessShader(fssrc);

    const char* c_vs = vs_proc.c_str();
    const char* c_fs = fs_proc.c_str();
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
        mork::error_logger(vs_proc);
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
        mork::error_logger(fs_proc);
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

const Uniform& Program::getUniform(const std::string& name) const {
    auto entry = uniforms.find(name);
    if(entry==uniforms.end()) {
        mork::error_logger("Tried to acess uniform \"", name, "\" which is not active in program ", _programID);
        throw std::runtime_error("Error acessing Uniform, see log");
    } 
    
    return entry->second;
}
 

}
