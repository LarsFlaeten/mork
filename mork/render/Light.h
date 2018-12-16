#ifndef _MORK_LIGHT_H_
#define _MORK_LIGHT_H_

#include <string>
#include <mork/math/vec3.h>
#include <mork/render/Program.h>

namespace mork {

struct AttenuationModel {
    double constant;
    double linear;
    double quadratic;
};

struct ColorModel {
    vec3d ambient;
    vec3d diffuse;
    vec3d specular;
};

class Light {
    public:
        Light(); 
        Light( const mork::vec3d& ambient,
              const mork::vec3d& diffuse,
              const mork::vec3d& specular);
        Light( const ColorModel& color);
       
        virtual void setAmbientColor(const mork::vec3d& col);
        virtual void setDiffuseColor(const mork::vec3d& col);
        virtual void setSpecularColor(const mork::vec3d& col);

        virtual mork::vec3d getAmbientColor() const;
        virtual mork::vec3d getDiffuseColor() const;
        virtual mork::vec3d getSpecularColor() const;



        virtual void set(const Program& prog, const std::string& target);
        

        static const AttenuationModel DEFAULT_ATTENUATION;

        static const ColorModel DEFAULT_COLOR;
    protected:
        vec3d ambient;
        vec3d diffuse;
        vec3d specular;  
};

class DirLight : public Light {
    public:
        
        // Default CTOR with default light and default direction (1, 0, 0);
        DirLight();
    
        DirLight( const mork::vec3d& ambient,
              const mork::vec3d& diffuse,
              const mork::vec3d& specular,
              const mork::vec3d& direction);
        
        virtual void setDirection(const vec3d& _dir);

        virtual vec3d getDirection() const;
         
        virtual void set(const Program& prog, const std::string& target);
    private:
        vec3d dir;
};

class PointLight : public Light {
    public:
        // Default CTOR with default color, attenuation and position(0,0,0)
        PointLight();

        PointLight( const mork::vec3d& ambient,
              const mork::vec3d& diffuse,
              const mork::vec3d& specular,
              const mork::vec3d& position);
        
        virtual void setPosition(const vec3d& pos);

        virtual vec3d getPosition() const;

        virtual void set(const Program& prog, const std::string& target);

        virtual void setAttenuation(const AttenuationModel& attenuation);
        virtual AttenuationModel getAttenuation() const;

    protected:
        vec3d position;
        AttenuationModel attenuation;
};

class SpotLight : public Light {
    public:
        
        // Only default constructor
        // Most likely default light, and position and direction changes all the time anyway
        SpotLight();

        virtual void setPosition(const vec3d& pos);

        virtual vec3d getPosition() const;

        virtual void setDirection(const vec3d& _dir);

        virtual vec3d getDirection() const;
        
        virtual void setAttenuation(const AttenuationModel& attenuation);
       
        virtual AttenuationModel getAttenuation() const;

        virtual void setAngles(double outer, double inner);
        virtual double getOuterAngle() const;
        virtual double getInnerAngle() const;


        
        virtual void set(const Program& prog, const std::string& target);

        
  protected:
        double  outerAngle;
        double  innerAngle;
        vec3d position;
        vec3d direction;
        AttenuationModel attenuation;

};


}

#endif
