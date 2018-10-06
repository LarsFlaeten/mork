#ifndef _MORK_CAMERA_H_
#define _MORK_CAMERA_H_

#include "mork/scene/SceneNode.h"

namespace mork {

    class Camera : public SceneNode {

        public:
            virtual void setPosition(const vec3d& pos);
            virtual void setRotation(const mat3d& rot);
            virtual void setRotation(const vec3d& look_dir, const vec3d& up_dir);
            virtual mat3d getRotation() const;
            virtual vec3d getPosition() const;

        protected:
            // Hide these and add empty overrides, to avoid setting children on cameras
            virtual void addChild(std::shared_ptr<SceneNode>& child);
            virtual void addChild(SceneNode&& child);




    };




}


#endif
