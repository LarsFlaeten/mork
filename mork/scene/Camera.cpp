#include "mork/scene/Camera.h"

#include <stdexcept>

namespace mork {

    void Camera::addChild(std::shared_ptr<SceneNode>& child) {
        throw std::runtime_error("Not implemented");
    }

    void Camera::addChild(SceneNode&& child) {
        throw std::runtime_error("Not implemented");
    }

    void Camera::setPosition(const vec4d& pos) {
        // Preserve current rotation:
        // TODO: localToParent or LocalToWorld?
        mat4d rot = mat4d(this->getRotation());
        
        mat4d trans = mat4d::translate(pos.xyz());

        this->setLocalToParent(trans * rot);
  
    }
    void Camera::setRotation(const mat3d& _rot) {
        mat4d trans = mat4d::translate(this->getPosition().xyz());
        mat4d rot = mat4d(_rot);
        this->setLocalToParent(trans * rot);
    }

    void Camera::lookAt(const vec3d& look_dir, const vec3d& up_dir) {
        vec3d forward= look_dir.normalize();
        vec3d up = up_dir.normalize();
        vec3d left = up_dir.crossProduct(look_dir);
        up = look_dir.crossProduct(left);

        mat3d rot3 = mat3d::IDENTITY;
        // Set in transposed mode to avoid making setRow methods on mat3:
        rot3.setColumn(0, -left);
        rot3.setColumn(1, up);
        rot3.setColumn(2, -forward);
        mat4d rot = mat4d(rot3.transpose());

        vec4d pos = this->getPosition();
        mat4d trans = mat4d::translate(-(pos.xyz()));
        
        // We now have thew view matrix:
        // https://learnopengl.com/Getting-started/Camera
        mat4d view = rot * trans;

        // the local to parent is the inverse of this matrix:
        this->setLocalToParent(view.inverse());
                
    }
     
    void Camera::lookAt(const vec4d& position, const vec4d& target, const vec3d& up_dir) {
        this->setPosition(position);

        vec3d look_dir = (position - target).xyz();
        lookAt(look_dir, up_dir);
    
    }


    mat3d Camera::getRotation() const {
        return this->getLocalToParent().mat3x3();
    }

    vec4d Camera::getPosition() const {
        return vec4d(this->getLocalToParent().translation());
    }

    void Camera::update() {
        // set this camera(node)s local to world by using its reference (if it exists)
        if(reference != nullptr) {
            this->setLocalToWorld(reference->getLocalToWorld());
        } else {
            this->setLocalToWorld(mat4d::IDENTITY);
        }       
    }

    mat4d Camera::getViewMatrix() const {

        return this->getWorldToLocal();
    }
 
    void Camera::setReference(std::shared_ptr<SceneNode> node) {
        reference = node;
    }
    
    std::shared_ptr<SceneNode> Camera::getReference() const {
        return reference;
    }
 
}
