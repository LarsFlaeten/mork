#ifndef _MORK_SCENENODE_H_
#define _MORK_SCENENODE_H_

#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>

#include "mork/math/mat4.h" 

#include "mork/scene/Frustum.h"
#include "mork/render/Program.h"
namespace mork {


    class SceneNode {
        public:
            // Initialize a SceneNode with the given ame
            SceneNode(const std::string& name);
            
            // Initialize a SceneNode with a random name
            SceneNode();
            

            // Takes ownership of a node, adds it to its children,
            // and returns a reference to the child (if further needed by caller)
            virtual SceneNode& addChild(SceneNode&& child);
            
            // Takes ownership of a node, adds it to its children,
            // and returns a reference to the child (if further needed by caller)
            virtual SceneNode& addChild(std::unique_ptr<SceneNode> child);

            // Returns a refernce to a named child. Throw exception if the name is not a child
            // of this SceneNode.
            virtual SceneNode& getChild(const std::string& name);

            // Returns a refernce to a vector of children references.
            virtual const std::vector<std::reference_wrapper<SceneNode> > & getChildren() const;

            // Removes all children
            virtual void clearChildren();


            // Removes the given SceneNode from the internal store of children, and returns the objecte
            virtual std::unique_ptr<SceneNode> removeChild(const SceneNode& node);

            virtual bool hasChild(const SceneNode& node) const;
            //virtual bool hasChild2(const SceneNode& node) const;
            virtual bool hasChild(const std::string& name) const;

            virtual bool operator==(const SceneNode& other) const;
            virtual bool operator!=(const SceneNode& other) const;

            // Lists all the children this node has
            virtual std::vector<std::string> listChildren() const;

            virtual const std::string& getName() const;

            virtual mat4d   getLocalToParent() const;

            virtual void    setLocalToParent(const mat4d& m);

            virtual mat4d   getLocalToWorld() const;
            virtual mat4d   getWorldToLocal() const;

            virtual vec3d   getWorldPos() const;

            virtual box3d   getWorldBounds() const;

            virtual void    setLocalBounds(const box3d& bounds);
            virtual void    enlargeLocalBounds(const box3d& bounds);

            virtual void    updateLocalToWorld(const mat4d& parentLocalToWorld);

            virtual bool isVisible() const;

            virtual void isVisible(bool visible);

            virtual void draw(const Program& prog) const;

        protected:
            std::string name;

            bool visible;

            mat4d   localToParent;

            mat4d   localToWorld;

            vec3d   worldPos;

            box3d   localBounds;

            box3d   worldBounds;

            // This is the hash map of the actual children store
            std::unordered_map<std::string, std::unique_ptr<SceneNode> > childrenMap;

            // This vector provides a convenience list of children references for iterating over children
            std::vector<std::reference_wrapper<SceneNode> > childrenRefs;
           
    };

    
}

#endif
