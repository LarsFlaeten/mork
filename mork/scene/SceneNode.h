#ifndef _MORK_SCENENODE_H_
#define _MORK_SCENENODE_H_

#include <vector>
#include <memory>

#include "mork/math/mat4.h" 
namespace mork {


    class SceneNode {
        public:
            SceneNode();
            
            virtual void addChild(std::shared_ptr<SceneNode> child);
            virtual void addChild(SceneNode&& child);


            virtual mat4d   getLocalToParent() const;

            virtual void    setLocalToParent(const mat4d& m);

            virtual mat4d   getLocalToWorld() const;
            virtual mat4d   getWorldToLocal() const;


            virtual void    setLocalToWorld(const mat4d& parentLocalToWorld);

        protected:

            mat4d   localToParent;

            mat4d   localToWorld;
            
            std::vector<std::shared_ptr<SceneNode> > children;

           
    };

    
}

#endif
