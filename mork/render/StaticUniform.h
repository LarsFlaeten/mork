#ifndef _MORK_STATICUNIFORM_H_
#define _MORK_STATICUNIFORM_H_

#include "mork/render/Uniform.h"

// Inspired by https://codereview.stackexchange.com/questions/40117/type-safe-program-uniform-manipulation-in-opengl
namespace mork {

  
    // Statically type safe Uniform 
    template<typename T> class StaticUniform {
        public:
            StaticUniform(const mork::Program& prog, const std::string& name);

            int getUniformLocation() const {
                return uniform_loc;
            }

            void set(const T& value);

        private:
            int uniform_loc;
            //UniformHandler<T>   uniformHandler;


    };


    template<typename T>
    StaticUniform<T>::StaticUniform(const mork::Program& prog, const std::string& name) {
        uniform_loc = glGetUniformLocation(prog.getProgramId(), name.c_str());
    }
    
    template<typename T>
    void StaticUniform<T>::set(const T& v) {
        UniformHandler<T>::set(v, uniform_loc); 
    }


}
#endif
