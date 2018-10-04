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
        if(type!=GL_INT && type!=GL_SAMPLER_2D) {
            mork::error_logger("Type of the Uniform was: ", type, ", but instance of template allows ", GL_INT, ", ", GL_SAMPLER_2D );
            throw std::runtime_error("Tried setting int on Uniform with different type");
        
        }
            
        UniformHandler<int>::set(i, u_loc);
    }




}
