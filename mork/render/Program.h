#ifndef _MORK_PROGRAM_H_
#define _MORK_PROGRAM_H_

#include <string>
#include <unordered_map>

#include "mork/render/Uniform.h"

namespace mork {

// Represents a compiled set of shaders linked as a OpenGL program

class Program {

public:

    Program(const std::string& vssrc, const std::string& fssrc);
    
    ~Program();

    Program(Program&& o);
    Program& operator=(Program&& o);

    void buildShaders(const std::string& vssrc, const std::string& fssrc);
 
    // TODO: remove and move use program to frambuffer?
    void    use() const;
    
    const Uniform& getUniform(const std::string& name) const;
    
    //protected:
    int getProgramID() const;

    static std::string   preProcessShader(const std::string& s);


private:

    int _programID;
    int _vs;
    int _fs;
    
    std::unordered_map<std::string, Uniform> uniforms;

};



}

#endif
