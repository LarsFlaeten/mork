#include "mork/scene/Camera.h"

#include <stdexcept>

namespace mork {
    Camera::Camera() :

        reference(nullptr), fov(radians(45.0)), aspect(800.0/600.0), near_clipping(0.1), far_clipping(100) {
        updateProjection();    
    }

    Camera::Camera(double _fov, double _aspect, double _near, double _far) :
        reference(nullptr), fov(_fov), aspect(_aspect), near_clipping(_near), far_clipping(_far) {
        updateProjection();
    } 

    mat4d Camera::getProjectionMatrix() const {
        return projection;
    }

    // Set FOV (in radians)
    void Camera::setFOV(double _fov) {
        fov = _fov;
        updateProjection();
    }
    // gets FOV in radians
    double Camera::getFOV() const {
        return fov;
    }

    void Camera::setAspectRatio(double width, double height) {
        aspect = width / height;
        updateProjection();
    }
    double Camera::getAspectRatio() const {
        return aspect;
    }

    double Camera::getFarClippingPlane() const {
        return far_clipping;
    }

    double Camera::getNearClippingPlane() const {
        return near_clipping;
    }
    void Camera::setClippingPlanes(double _near, double _far) {
        near_clipping = _near;
        far_clipping = _far;
        updateProjection();
    }

    void Camera::updateProjection() {
        projection = mork::mat4d::perspectiveProjection(fov, aspect, near_clipping, far_clipping); 
        
    }


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
        
        // We now have the view matrix (in parent reference frame):
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
    
    // Local camera fron is 0,0,-1, so tranform to world coordinates
    vec3d Camera::getWorldForward() const {
        return this->getLocalToWorld().mat3x3()*vec3d(0,0,-1);

    }
 
    // Local camera up is 0,1,0, so tranform to world coordinates
    vec3d Camera::getWorldUp() const {
        return this->getLocalToWorld().mat3x3()*vec3d(0,1,0);

    }
     
    // Local camera right is 1,0,0, so tranform to world coordinates
    vec3d Camera::getWorldRight() const {
        return this->getLocalToWorld().mat3x3()*vec3d(1,0,0);

    }
 
    void Camera::update() {
        // set this camera(node)s local to world by using its reference (if it exists)
        if(reference != nullptr) {
            this->updateLocalToWorld(reference->getLocalToWorld());
        } else {
            this->updateLocalToWorld(mat4d::IDENTITY);
        }

        // Get world to Screen and compute frustum planes
        worldToScreen = projection * this->getWorldToLocal();
        worldFrustum.setPlanes(worldToScreen);

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
    
    const Frustum& Camera::getWorldFrustum() const {
        return worldFrustum;
    }

    const mat4d& Camera::getWorldToScreen() const {
        return worldToScreen;
    } 
}
