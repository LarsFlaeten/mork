#include "mork/scene/Camera.h"

namespace mork {

    void Camera::addChild(std::shared_ptr<SceneNode>& child) {
    }

    void Camera::addChild(SceneNode&& child) {
    }

    void Camera::setPosition(const vec3d& pos) {
        // Preserve current rotation:
        // TODO: localToParent or LocalToWorld?
        mat3d rot = localToParent.mat3x3();
        
        mat4d trans = mat4d::translate(pos);
    }
    void Camera::setRotation(const mat3d& rot) {
    
    }

    void Camera::setRotation(const vec3d& look_dir, const vec3d& up_dir) {

    }
    
    mat3d Camera::getRotation() const {

    }

    vec3d Camera::getPosition() const {

    }



}
