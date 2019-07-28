#ifndef _MORK_CAMERA_H_
#define _MORK_CAMERA_H_

#include "mork/scene/SceneNode.h"

#include "mork/scene/Frustum.h"

namespace mork {

    class Camera : public SceneNode {

        public:
            enum Mode {
                // Free camerae mode, set position and rotations manually, alternatively lookAt
                FREE,
                // Orbit mode - Focused on the reference object, and setters for azimuth, elevation and distance. Up vector is implicitly calculated by orthogonality
                ORBIT


            }; 

            Camera();
            Camera(const std::string& name);
            Camera(double fov, double aspect, double near, double far);
            Camera(const std::string& name, double fov, double aspect, double near, double far);

            virtual void setMode(Mode _mode);
            virtual Mode getMode() const;

            // Free camera setters
            virtual void setPosition(const vec3d& pos);
            virtual void setRotation(const mat3d& rot);
            virtual void lookAt(const vec3d& look_dir, const vec3d& up_dir);
            virtual void lookAt(const vec3d& position, const vec3d& target, const vec3d& up);

            // Orbit camera setters
            virtual void setAzimuth(double radians);
            virtual void setElevation(double radians);
            virtual void setDistance(double distance);
            virtual vec3d getFocusPosition() const;
            virtual double getAzimuth() const;
            virtual double getElevation() const;
            virtual double getDistance() const;




            // General position/rotation getters:
            virtual mat3d getRotation() const;
            virtual vec3d getPosition() const;
            
            virtual vec3d getWorldForward() const;
            virtual vec3d getWorldUp() const;
            virtual vec3d getWorldRight() const;
            virtual mat3d getWorldRotation() const;
            virtual vec3d getWorldPosition() const;
            
            virtual mat4d getViewMatrix() const;
            virtual mat4d getProjectionMatrix() const;

            // View/screen related getters/setters
            virtual void setFOV(double fov);
            virtual double getFOV() const;

            virtual void setAspectRatio(double width, double height);
            virtual double getAspectRatio() const;

            virtual double getFarClippingPlane() const;
            virtual double getNearClippingPlane() const;
            virtual void setClippingPlanes(double near, double far);


            virtual void update();

            virtual void setReference(const SceneNode& node);
            virtual void clearReference();


            virtual const SceneNode& getReference() const;

            virtual const Frustum& getWorldFrustum() const;

            virtual const mat4d& getWorldToScreen() const;
        protected:
            virtual void updateProjection();

            // Hide these and add empty overrides, to avoid setting children on cameras
            virtual SceneNode& addChild(SceneNode&& child);

            // This is hidde so that cameras cannot be made children of other nodes. 
            // Cameras use their reference nodes to calculate global position, if it exist.           
            virtual void    updateLocalToWorld(const mat4d& parentLocalToWorld);



            // Recalcualtes localToParent based on azimtuh, elevation and distance
            void recalcOrbit();

            const SceneNode*  reference;
            
            Frustum worldFrustum;            
            Mode    mode; 
            mat4d projection;
            mat4d worldToScreen;
            double fov, aspect, far_clipping, near_clipping;

            // Orbit mode properties;
            double az_rad, el_rad, distance;

    };




}


#endif
