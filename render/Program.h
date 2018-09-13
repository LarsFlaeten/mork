#ifndef _MORK_PROGRAM_H_
#define _MORK_PROGRAM_H_

#include <string>

namespace mork {

// Represents a compiled set of shaders linked as a OpenGL program

class Program {

public:

    Program(const std::string& vssrc, const std::string& fssrc);
    
    ~Program();

    // TODO: remove and move use program to frambuffer
    void    use() const;
//protected:
    int getProgramID() const;

private:
    int _programId;
};



}

#endif
