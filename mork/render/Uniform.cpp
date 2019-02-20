#include "mork/render/Uniform.h"
#include "mork/glad/glad.h"
namespace mork {
    Uniform::Uniform(int _type, int _uniformLocation)
        : type(_type), u_loc(_uniformLocation) {
        
    }

    void Uniform::set(const mork::vec3f& v) const {
        if(type!=GL_FLOAT_VEC3) {
            mork::error_logger("Type was: ", type, ", tried setting: ", GL_FLOAT_VEC3, "(GL_FOAT_VEC3)");
            throw std::runtime_error("Tried setting vec3f on Uniform with different type");
        }
        UniformHandler<mork::vec3f>::set(v, u_loc);
    }
    void Uniform::set(const mork::vec4f& v) const {
        if(type!=GL_FLOAT_VEC4) {
            mork::error_logger("Type was: ", type, ", tried setting: ", GL_FLOAT_VEC4, "(GL_FOAT_VEC4)");
            throw std::runtime_error("Tried setting vec4f on Uniform with different type");
        }
        UniformHandler<mork::vec4f>::set(v, u_loc);
    }
    void Uniform::set(const int& i) const {
        if(type!=GL_INT && type!=GL_SAMPLER_2D && type != GL_SAMPLER_CUBE) {
            mork::error_logger("Type of the Uniform was: ", type, ", but instance of template allows ", GL_INT, ", ", GL_SAMPLER_2D );
            throw std::runtime_error("Tried setting int on Uniform with different type");
        
        }
            
        UniformHandler<int>::set(i, u_loc);
    }

    void Uniform::set(const unsigned int& i) const {
        if(type!=GL_UNSIGNED_INT) {
            mork::error_logger("Type of the Uniform was: ", type, ", but instance of template allows ", GL_UNSIGNED_INT );
            throw std::runtime_error("Tried setting int on Uniform with different type");
        
        }
            
        UniformHandler<unsigned int>::set(i, u_loc);
    }


    void Uniform::set(const mork::mat4f& m) const {
        if(type!=GL_FLOAT_MAT4) {
            mork::error_logger("Type was: ", type, ", tried setting: ", GL_FLOAT_MAT4, "(GL_FOAT_MAT4)");
            throw std::runtime_error("Tried setting mat4f on Uniform with different type");
        }
        UniformHandler<mork::mat4f>::set(m, u_loc);
    }
 
    void Uniform::set(const mork::mat3f& m) const {
        if(type!=GL_FLOAT_MAT3) {
            mork::error_logger("Type was: ", type, ", tried setting: ", GL_FLOAT_MAT3, "(GL_FOAT_MAT3)");
            throw std::runtime_error("Tried setting mat3f on Uniform with different type");
        }
        UniformHandler<mork::mat3f>::set(m, u_loc);
    }
 
    void Uniform::set(const float& f) const {
        if(type!=GL_FLOAT) {
            mork::error_logger("Type was: ", type, ", tried setting: ", GL_FLOAT, "(GL_FOAT)");
            throw std::runtime_error("Tried setting float on Uniform with different type");
        }
        UniformHandler<float>::set(f, u_loc);
    }
 
}
