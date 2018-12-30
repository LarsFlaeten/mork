#ifndef _MORK_CAMERA_H_
#define _MORK_CAMERA_H_

#include "mork/scene/SceneNode.h"

#include "mork/scene/Frustum.h"

namespace mork {

    class Camera : public SceneNode {

        public:
            Camera();
            Camera(double fov, double aspect, double near, double far);

            virtual void setPosition(const vec4d& pos);
            virtual void setRotation(const mat3d& rot);
            virtual void lookAt(const vec3d& look_dir, const vec3d& up_dir);
            virtual void lookAt(const vec4d& position, const vec4d& target, const vec3d& up);
            virtual mat3d getRotation() const;
            virtual vec4d getPosition() const;
            
            virtual vec3d getWorldForward() const;
            virtual vec3d getWorldUp() const;
            virtual vec3d getWorldRight() const;
            
            virtual mat4d getViewMatrix() const;
            virtual mat4d getProjectionMatrix() const;

            // Set FOV (in radians)
            virtual void setFOV(double fov);
            // gets FOV in radians
            virtual double getFOV() const;

            virtual void setAspectRatio(double width, double height);
            virtual double getAspectRatio() const;

            virtual double getFarClippingPlane() const;
            virtual double getNearClippingPlane() const;
            virtual void setClippingPlanes(double near, double far);


            virtual void update();

            virtual void setReference(std::shared_ptr<SceneNode> node);
            virtual std::shared_ptr<SceneNode> getReference() const;

            virtual const Frustum& getWorldFrustum() const;

            virtual const mat4d& getWorldToScreen() const;
        protected:
            virtual void updateProjection();

            // Hide these and add empty overrides, to avoid setting children on cameras
            virtual void addChild(std::shared_ptr<SceneNode>& child);
            virtual void addChild(SceneNode&& child);


            std::shared_ptr<SceneNode>  reference;
            
            Frustum worldFrustum;            
            
            mat4d projection;
            mat4d worldToScreen;
            double fov, aspect, far_clipping, near_clipping;
    };




}


#endif
