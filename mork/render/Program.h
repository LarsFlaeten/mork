#ifndef _MORK_PROGRAM_H_
#define _MORK_PROGRAM_H_

#include <string>
#include <vector>
#include <unordered_map>

#include "mork/render/Uniform.h"
#include "mork/render/Texture.h"

namespace mork {

// An opengl shader object
class Shader {

    public:
        enum Type {
            VERTEX,
            FRAGMENT,
            GEOMETRY
        };   
    public:
        Shader(int version, const std::string& src, Type type, const std::string& define);  
        ~Shader();
        // Prevent copying
        Shader(Shader& o) = delete;
        Shader& operator=(Shader& o) = delete;
        // allow move
        Shader(Shader&& o) noexcept;
        Shader& operator=(Shader&& o) noexcept;

        void buildShader(const std::string& src);
        static std::string preProcess(const std::string& src);

        Type getType() const;
        int  getId() const;
    private:
        int _id;
        Type _type;
};
    
    
    
    // Represents a compiled set of shaders linked as a OpenGL program

class Program {

public:
    // creates an empty program. Must be linked with buildProgram before use
    Program();

    // Build a shader program from a combined shader source file
    // (Using directives such as #ifdef _VERTEX_, GEOMETRY_, _FRAGMENT_)
    Program(int version, const std::string& src_path);

    Program(const std::string& vssrc, const std::string& fssrc);
    Program(const std::string& vssrc, const std::string& gssrc, const std::string& fssrc);
    
    ~Program();

    Program(Program&& o) noexcept;
    Program& operator=(Program&& o) noexcept;

    void buildProgram(const std::vector<std::reference_wrapper<Shader> >& shaders);
 
    // TODO: remove and move use program to frambuffer?
    void    use() const;

    // Returns the specified uniform from the program
    // Throws an exception if the uniform does not exist    
    const Uniform& getUniform(const std::string& name) const;
 
    // Queries wether a uniform exist
    bool queryUniform(const std::string& name) const;
    
    //protected:
    int getProgramId() const;

    // Binds a texture to the given name and texture unit in the shader
    bool bindTexture(const TextureBase& tex, const std::string& name, int texUnit) const;

private:

    int _programID;

    std::unordered_map<std::string, Uniform> uniforms;

};


// a resource pool of programs
// A simple unordered map wrapper, with resource initialization in the source file
using ProgramPool = std::unordered_map<std::string, Program>;


}

#endif
