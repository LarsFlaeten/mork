#ifndef _MORK_SCENE_H_
#define _MORK_SCENE_H_

#include <vector>

#include "mork/math/mat4.h" 
#include "mork/scene/SceneNode.h"
#include "mork/scene/Camera.h"

namespace mork {

    class Scene {
        public:
            Scene();
            
            virtual ~Scene();

            const SceneNode& getRoot() const;

            SceneNode& getRoot();

            void    update();

            void addCamera(std::shared_ptr<Camera> camera);

        private:
            SceneNode   root;            

            std::vector<std::shared_ptr<Camera> > cameras;

    };

    
}

#endif