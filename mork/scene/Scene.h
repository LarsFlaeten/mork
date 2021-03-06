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
            Scene(Scene& o) = delete;
            Scene& operator=(Scene& o) = delete;

            Scene(Scene&& o) = default;
            Scene& operator=(Scene&& o) = default;

            ~Scene();

            const SceneNode& getRoot() const;

            SceneNode& getRoot();
            
            const Camera& getCamera() const;

            Camera& getCamera();


            void    update();

            void    draw(const Program& prog);


        private:

            void computeVisibility(const Camera& cam, SceneNode& node, Visibility parentVisibility);

            SceneNode   root;            

            Camera      camera;

    };

    
}

#endif
