#ifndef _MORK_PROGRAM_H_
#define _MORK_PROGRAM_H_

#include <string>
#include <unordered_map>

#include "mork/render/Uniform.h"

namespace mork {

// Represents a compiled set of shaders linked as a OpenGL program

class Program {

public:

    // Build a shader program from a combined shader source file
    // (Using directives such as #ifdef _VERTEX_, GEOMETRY_, _FRAGMENT_)
    Program(int version, const std::string& src_path);

    Program(const std::string& vssrc, const std::string& fssrc);
    Program(const std::string& vssrc, const std::string& gssrc, const std::string& fssrc);
    
    ~Program();

    Program(Program&& o);
    Program& operator=(Program&& o);

    void buildShaders(const std::string& vssrc, const std::string& fssrc, const std::string& gssrc = "");
 
    // TODO: remove and move use program to frambuffer?
    void    use() const;

    // Returns the specified uniform from the program
    // Throws an exception if the uniform does not exist    
    const Uniform& getUniform(const std::string& name) const;
 
    // Queries wether a uniform exist
    bool queryUniform(const std::string& name) const;
    
    //protected:
    int getProgramID() const;

    static std::string   preProcessShader(const std::string& s);

private:

    int _programID;
    int _vs;
    int _fs;
    int _gs;

    std::unordered_map<std::string, Uniform> uniforms;

};


// a resource pool of programs
// A simple unordered map wrapper, with resource initialization in the source file
using ProgramPool = std::unordered_map<std::string, Program>;


}

#endif
