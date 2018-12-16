#include "mork/render/Light.h"

namespace mork {

    Light::Light( const mork::vec3d& _ambient,
              const mork::vec3d& _diffuse,
              const mork::vec3d& _specular) {
        setAmbientColor(_ambient);
        setDiffuseColor(_diffuse);
        setSpecularColor(_specular);



    }
    
    Light::Light( const ColorModel& _color) {
        setAmbientColor(_color.ambient);
        setDiffuseColor(_color.diffuse);
        setSpecularColor(_color.specular);



    }
       
    void Light::setAmbientColor(const mork::vec3d& col) {
        ambient = col;
    }

    void Light::setDiffuseColor(const mork::vec3d& col) {
        diffuse = col;
    }

    void Light::setSpecularColor(const mork::vec3d& col) {
        specular = col;
    }

    mork::vec3d Light::getAmbientColor() const {
        return ambient;
    }

    mork::vec3d Light::getDiffuseColor() const {
        return diffuse;
    }

    mork::vec3d Light::getSpecularColor() const {
        return specular;
    }


    void    Light::set(const Program& prog, const std::string& target) {
        prog.getUniform(target + ".ambient").set(ambient.cast<float>());
        prog.getUniform(target + ".diffuse").set(diffuse.cast<float>());
        prog.getUniform(target + ".specular").set(specular.cast<float>());
    }

    const AttenuationModel Light::DEFAULT_ATTENUATION = AttenuationModel( {1.0, 0.09, 0.0032} );
    
    const ColorModel Light::DEFAULT_COLOR = ColorModel(
            {vec3d(0.2, 0.2, 0.2),
             vec3d(1.0, 1.0, 1.0),
             vec3d(1.0, 1.0, 1.0)});

    PointLight::PointLight() 
        : position(vec3d::ZERO), Light(Light::DEFAULT_COLOR), attenuation(Light::DEFAULT_ATTENUATION) {

    }
 
    PointLight::PointLight( const mork::vec3d& _ambient,
              const mork::vec3d& _diffuse,
              const mork::vec3d& _specular,
              const mork::vec3d& _position) 
        : position(_position), Light(_ambient, _diffuse, _specular), attenuation(Light::DEFAULT_ATTENUATION) {

    }
        
    void PointLight::setPosition(const vec3d& pos) {
        position = pos;
    }

    vec3d PointLight::getPosition() const {
        return position;
    }

    void PointLight::set(const Program& prog, const std::string& target) {
        prog.getUniform(target + ".position").set(position.cast<float>());
        prog.getUniform(target + ".constant").set(static_cast<float>(attenuation.constant));
        prog.getUniform(target + ".linear").set(static_cast<float>(attenuation.linear));
        prog.getUniform(target + ".quadratic").set(static_cast<float>(attenuation.quadratic));




        Light::set(prog, target);
    }

    void PointLight::setAttenuation(const AttenuationModel& _att) {
        attenuation = _att;
    }

    AttenuationModel PointLight::getAttenuation() const {
        return attenuation;
    }
    
    DirLight::DirLight() 
        : dir(vec3d(1,0,0)), Light(Light::DEFAULT_COLOR) {

    }
    
    DirLight::DirLight( const mork::vec3d& _ambient,
              const mork::vec3d& _diffuse,
              const mork::vec3d& _specular,
              const mork::vec3d& _direction) 
        : dir(_direction), Light(_ambient, _diffuse, _specular) {

    }
        
    void DirLight::setDirection(const vec3d& _dir) {
        dir = _dir;
    }

    vec3d DirLight::getDirection() const {
        return dir;
    }

    void DirLight::set(const Program& prog, const std::string& target) {
        prog.getUniform(target + ".direction").set(dir.cast<float>());
        Light::set(prog, target);
    }


    SpotLight::SpotLight() 
        : position(vec3d::ZERO), direction(vec3d(1,0,0)), Light(Light::DEFAULT_COLOR), attenuation(Light::DEFAULT_ATTENUATION), outerAngle(radians(14.5)), innerAngle(radians(12.5)) {

    }
 
    void SpotLight::setAttenuation(const AttenuationModel& _att) {
        attenuation = _att;
    }

    AttenuationModel SpotLight::getAttenuation() const {
        return attenuation;
    }
     
    void SpotLight::setDirection(const vec3d& _dir) {
        direction = _dir;
    }

    vec3d SpotLight::getDirection() const {
        return direction;
    }

    void SpotLight::setPosition(const vec3d& pos) {
        position = pos;
    }

    vec3d SpotLight::getPosition() const {
        return position;
    }
    
    void SpotLight::setAngles(double outer, double inner) {
        outerAngle = outer;
        innerAngle = inner;
    }

    double SpotLight::getOuterAngle() const {
        return outerAngle;
    }

    double SpotLight::getInnerAngle() const {
        return innerAngle;
    }




    void SpotLight::set(const Program& prog, const std::string& target) {
        prog.getUniform(target + ".position").set(position.cast<float>());
        prog.getUniform(target + ".direction").set(direction.cast<float>());
        prog.getUniform(target + ".constant").set(static_cast<float>(attenuation.constant));
        prog.getUniform(target + ".linear").set(static_cast<float>(attenuation.linear));
        prog.getUniform(target + ".quadratic").set(static_cast<float>(attenuation.quadratic));
        prog.getUniform(target + ".outerCutOff").set(cos(static_cast<float>(outerAngle)));
        prog.getUniform(target + ".cutOff").set(cos(static_cast<float>(innerAngle)));
   



        Light::set(prog, target);
    }


}
