#ifndef _MORK_UNIFORM_H_
#define _MORK_UNIFORM_H_

#include "mork/glad/glad.h"
#include "mork/math/vec4.h"
#include "mork/math/mat4.h"
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

            void set(const vec2f& v) const;
            void set(const vec3f& v) const;
            void set(const vec4f& v) const;
            void set(const int& i) const;
            void set(const unsigned int& i) const;
            void set(const mat4f& m) const;
            void set(const mat3f& m) const;
            void set(const float& f) const;
            
            // Implicit cast to float:
            void set(const double& f) const;


            int getType() const;
        private:
            int type;
            int u_loc;


    };


    template<> class UniformHandler<mork::vec2f>
    {
        public:
            inline static void set(const mork::vec2f& v, int u_loc) {
                glUniform2fv(u_loc, 1, &v.x);
            }
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
    
    template<> class UniformHandler<unsigned int>
    {
        public:
            inline static void set(const unsigned int& i, int u_loc) {
                glUniform1ui(u_loc, i);
            }
    };


    template<> class UniformHandler<mork::mat4f>
    {
        public:
            inline static void set(const mork::mat4f& m, int u_loc) {
                // We transpose the matrix (param 3 = GL_TRUE) since
                // mork::matX is row-major, but OpenGL is column-major.
                glUniformMatrix4fv(u_loc, 1, GL_TRUE, m.coefficients());
            }
    };

    template<> class UniformHandler<mork::mat3f>
    {
        public:
            inline static void set(const mork::mat3f& m, int u_loc) {
                // We transpose the matrix (param 3 = GL_TRUE) since
                // mork::matX is row-major, but OpenGL is column-major.
                glUniformMatrix3fv(u_loc, 1, GL_TRUE, m.coefficients());
            }
    };

    template<> class UniformHandler<float>
    {
        public:
            inline static void set(const float& f, int u_loc) {
                glUniform1f(u_loc, f);
            }

    };

}

#endif
