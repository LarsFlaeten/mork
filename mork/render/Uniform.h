#ifndef _MORK_UNIFORM_H_
#define _MORK_UNIFORM_H_

#include "mork/glad/glad.h"
#include "mork/math/vec4.h"
#include "mork/core/Log.h"
#include <string>
#include <typeinfo>
#include <stdexcept>


// Inspired by https://codereview.stackexchange.com/questions/40117/type-safe-program-uniform-manipulation-in-opengl
namespace mork {

    template <class T> class UniformHandler;
 
  
    // Dynamically type safe Uniform
    class Uniform {
        public:
            Uniform(int type, int uniformLocation);

            void set(const vec3f& v) const;
            void set(const vec4f& v) const;
            void set(const int& i) const;

        private:
            int type;
            int u_loc;


    };



    template<> class UniformHandler<mork::vec4f>
    {
        public:
            inline static void set(const mork::vec4f& v, int u_loc) {
                glUniform4fv(u_loc, 1, &v.x);
            }
    };

    template<> class UniformHandler<mork::vec3f>
    {
        public:
            inline static void set(const mork::vec3f& v, int u_loc) {
                glUniform3fv(u_loc, 1, &v.x);
            }
    };

    template<> class UniformHandler<int>
    {
        public:
            inline static void set(const int& i, int u_loc) {
                glUniform1i(u_loc, i);
            }
    };


}

#endif
