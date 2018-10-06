#ifndef _MORK_SCENE_H_
#define _MORK_SCENE_H_

#include <vector>

#include "mork/math/mat4.h" 
#include "mork/scene/SceneNode.h"

namespace mork {

    class Scene {
        public:
            Scene();
            
            virtual ~Scene();

            const SceneNode& getRoot() const;

            SceneNode& getRoot();

            void    update();

        private:
            SceneNode   root;            

    };

    
}

#endif
