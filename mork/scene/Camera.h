#ifndef _MORK_CAMERA_H_
#define _MORK_CAMERA_H_

#include "mork/scene/SceneNode.h"

namespace mork {

    class Camera : public SceneNode {

        public:
            virtual void setPosition(const vec4d& pos);
            virtual void setRotation(const mat3d& rot);
            virtual void lookAt(const vec3d& look_dir, const vec3d& up_dir);
            virtual void lookAt(const vec4d& position, const vec4d& target, const vec3d& up);
            virtual mat3d getRotation() const;
            virtual vec4d getPosition() const;

            virtual mat4d getViewMatrix() const;

            virtual void update();

            virtual void setReference(std::shared_ptr<SceneNode> node);
            virtual std::shared_ptr<SceneNode> getReference() const;
        protected:
            // Hide these and add empty overrides, to avoid setting children on cameras
            virtual void addChild(std::shared_ptr<SceneNode>& child);
            virtual void addChild(SceneNode&& child);


            std::shared_ptr<SceneNode>  reference;

    };




}


#endif
