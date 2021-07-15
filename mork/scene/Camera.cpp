#include "mork/scene/Camera.h"
#include "mork/util/Util.h"

#include <stdexcept>

namespace mork {
 
    Camera::Camera(const std::string& name, double _fov, double _aspect, double _near, double _far) :
        SceneNode(name), reference(nullptr), fov(_fov), aspect(_aspect), near_clipping(_near), far_clipping(_far), mode(FREE), az_rad(0.0), el_rad(0.0), distance(1.0) {
        updateProjection();
    } 

    Camera::Camera() :
        Camera(std::string("Cam") + random_string(5))
    {
    }
    
    Camera::Camera(const std::string& name) :
        Camera(name, radians(45.0), 800.0/600.0, 0.1, 100.0) {
    }

   
    Camera::Camera(double _fov, double _aspect, double _near, double _far) :
        Camera(std::string("Cam") + random_string(5), _fov, _aspect, _near, _far) {
    } 


    mat4d Camera::getProjectionMatrix() const {
        return projection;
    }
    
    void Camera::setMode(Mode _mode) {
        mode = _mode;
    }
    
    Camera::Mode Camera::getMode() const {
        return mode;
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

/*
    void Camera::addChild(std::shared_ptr<SceneNode>& child) {
        throw std::runtime_error("Not implemented");
    }
*/
    SceneNode& Camera::addChild(SceneNode&& child) {
        throw std::runtime_error("Not implemented");
    }

    void Camera::setPosition(const vec3d& pos) {
        if(mode == FREE) {
            // Preserve current rotation:
            // TODO: localToParent or LocalToWorld?
            mat4d rot = mat4d(this->getRotation());
        
            mat4d trans = mat4d::translate(pos);

            this->setLocalToParent(trans * rot);
        } else
            warn_logger("Tried setting position of camera not in FREE mode, ignoring");
  
    }
    void Camera::setRotation(const mat3d& _rot) {
        if(mode == FREE) {
            mat4d trans = mat4d::translate(this->getPosition());
            mat4d rot = mat4d(_rot);
            this->setLocalToParent(trans * rot);
        } else
            warn_logger("Tried setting rotation of camera not in FREE mode, ignoring");
    }

    void Camera::lookAt(const vec3d& look_dir, const vec3d& up_dir) {
        if(mode == FREE) {
            vec3d forward= look_dir.normalize();
            vec3d up = up_dir.normalize();
            vec3d left = (up_dir.crossProduct(look_dir)).normalize();
            up = (look_dir.crossProduct(left)).normalize();

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
        } else
            warn_logger("Tried setting position/rotation of camera not in FREE mode, ignoring");
                    
    }
     
    void Camera::lookAt(const vec3d& position, const vec3d& target, const vec3d& up_dir) {
        if(mode == FREE) {
            this->setPosition(position);

            vec3d look_dir = position - target;
            lookAt(look_dir, up_dir);
        } else
            warn_logger("Tried setting position/rotaion of camera not in FREE mode, ignoring");
     
    }

    void Camera::setAzimuthElevationDistance(double az_radians, double el_radians, double dist) {
        az_rad = az_radians;

        // clamp to +/- PI
        while(az_rad>M_PI)
            az_rad -= 2.0*M_PI;
        while(az_rad<-M_PI)
            az_rad += 2.0*M_PI;

		el_rad = el_radians;

        // clamp to +/- PI/2
        while(el_rad>0.5*M_PI)
            el_rad -= M_PI;
        while(el_rad<-0.5*M_PI)
            el_rad += M_PI;

		this->distance = distance;

		recalcOrbit();

    }

    void Camera::setAzimuth(double radians) {
        az_rad = radians;

        // clamp to +/- PI
        while(az_rad>M_PI)
            az_rad -= 2.0*M_PI;
        while(az_rad<-M_PI)
            az_rad += 2.0*M_PI;

        recalcOrbit();


    }
    
    void Camera::setElevation(double radians) {
        el_rad = radians;

        // clamp to +/- PI/2
        while(el_rad>0.5*M_PI)
            el_rad -= M_PI;
        while(el_rad<-0.5*M_PI)
            el_rad += M_PI;

        recalcOrbit();
    }
    
    void Camera::setDistance(double distance) {
        this->distance = distance;
        recalcOrbit();
    }
    
    vec3d Camera::getFocusPosition() const {
        if(reference == nullptr) {
            error_logger("Obtaning getFcosuPosition without refernce object set is not valid");
            throw std::runtime_error(error_logger.last());
        }

    }
    
    double Camera::getAzimuth() const {
        return az_rad;
    }
    
    double Camera::getElevation() const {
        return el_rad;
    }
    
    double Camera::getDistance() const {
        return distance;
    }

    void Camera::recalcOrbit() {
        if(mode != ORBIT) {
            error_logger("Recaluclating orbit camera without orbit mode set not allowed");
            throw std::runtime_error(error_logger.last());
        }

        if(reference == nullptr) {
            error_logger("Recaluclating orbit camera without having a reference node is not allowed");
            throw std::runtime_error(error_logger.last());
        }

        // Reference is the center:
        vec3d targetPos = vec3d::ZERO;
      
        // Camera pos 
        double x = cos(az_rad)*cos(el_rad)*distance;
        double y = sin(az_rad)*cos(el_rad)*distance; 
        double z = sin(el_rad)*distance;
        vec3d camPos(x, y, z);
        vec3d forwd = -camPos.normalize();
        vec3d right = forwd.crossProduct(vec3d(0.0, 0.0, 1.0));
        vec3d up = right.crossProduct(forwd).normalize();

        // Fake FREE, and use the lookAt method:
        mode = FREE;
        this->setPosition(camPos);
        lookAt(forwd, up);
        mode = ORBIT;
    }

    mat3d Camera::getRotation() const {
        return this->getLocalToParent().mat3x3();
    }

    vec3d Camera::getPosition() const {
        return this->getLocalToParent().translation();
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
     
    mat3d Camera::getWorldRotation() const {
        return this->getLocalToWorld().mat3x3();
    }

    vec3d Camera::getWorldPosition() const {
        return this->getLocalToWorld().translation();
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
    
    void    Camera::updateLocalToWorld(const mat4d& parentLocalToWorld) {
        SceneNode::updateLocalToWorld(parentLocalToWorld);
    }


    mat4d Camera::getViewMatrix() const {

        return this->getWorldToLocal();
    }
 
    void Camera::setReference(const SceneNode& node) {
        reference = &node;
    }
 
    void Camera::clearReference() {
        reference = nullptr;
    }
    
    const SceneNode& Camera::getReference() const {
        if(reference == nullptr)
            throw std::runtime_error("Nullpointer exception, Camera has no reference set");
        return *reference;
    }
    
    const Frustum& Camera::getWorldFrustum() const {
        return worldFrustum;
    }

    const mat4d& Camera::getWorldToScreen() const {
        return worldToScreen;
    } 
}
