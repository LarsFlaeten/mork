#include "mork/scene/SceneNode.h"
#include "mork/core/Log.h"
#include "mork/util/Util.h"
namespace mork {

    SceneNode::SceneNode(const std::string& name) 
        :   name(name),
            localToParent(mat4d::IDENTITY),
            localToWorld(mat4d::IDENTITY),
            localBounds(box3d::ZERO),
            worldBounds(box3d::ZERO),
            worldPos(vec3d::ZERO) {
    }

    SceneNode::SceneNode()
        : SceneNode(random_string(8)) {

    }

    SceneNode& SceneNode::addChild(SceneNode&& child) {
        std::string name = child.getName();
        
        // Check that the child name does not allready exist
        auto it = childrenMap.find(name);
        if(it != childrenMap.end()) {
           error_logger("Allready existing Child element " + name + " requested to be inserted in node " + this->getName());
           throw std::runtime_error("Child name allready existing");
        }

        auto& ret = childrenMap[name] = std::make_unique<SceneNode>(std::move(child));

        // Push to childrenRefs
        childrenRefs.push_back(*ret);

        return *ret;
    }

    SceneNode& SceneNode::addChild(std::unique_ptr<SceneNode> child) {
        // Check that the child name does not allready exist
        auto it = childrenMap.find(child->getName());
        if(it != childrenMap.end()) {
           error_logger("Allready existing Child element " + child->getName() + " requested to be inserted in node " + this->getName());
           throw std::runtime_error("Child name allready existing");
        }
        
        auto& ret = childrenMap[child->getName()] = std::move(child);
         
        // Push to childrenRefs:
        childrenRefs.push_back(*ret);

        return *ret;
    }
   
    SceneNode& SceneNode::getChild(const std::string& name) {
        auto it = childrenMap.find(name);
        if(it == childrenMap.end()) {
           error_logger("Non-existing Child element " + name + " requested from node " + this->getName());
           throw std::runtime_error("Child element not found");
        }
        return *((*it).second);
    }
    
    const std::vector<std::reference_wrapper<SceneNode> > & SceneNode::getChildren() const {
        return childrenRefs;
    }

    // Naive implementation - O(n)
    /*
    bool SceneNode::hasChild(const SceneNode& node) const {
        for(const SceneNode& child : childrenRefs)
            if(&child == &node)
               return true;

        return false; 
    }
  */  
    // Improved search, using the hasmap lookup O(1)
    bool SceneNode::hasChild(const SceneNode& node) const {
        auto it = childrenMap.find(node.getName());

        if(it != childrenMap.end()) {
            // Compare adresses
            if(((*it).second).get() == &node)
                return true;
            else // they just had the same name, not adress..
                return false;
        } else
            return false;
    }


    bool SceneNode::hasChild(const std::string& name) const {
        auto it = childrenMap.find(name);
        if(it != childrenMap.end())
            return true;
        else
            return false;
 
    }


    // Removes the given SceneNode from the internal store of children, and returns the object by move.
    // If the object is not in the children pool, an exception is thrown
    std::unique_ptr<SceneNode> SceneNode::removeChild(const SceneNode& node) {
        // First we create a raw pointer to the object and release it from the ownership of the uniqu_ptr
        // in the children hashmap store:
        auto it = childrenMap.find(node.getName());

        std::unique_ptr<SceneNode> extracted_ptr = nullptr;

        bool found = false;
        if(it != childrenMap.end()) {
            // Compare adresses
            if(((*it).second).get() == &node) {
               found = true;
            }
            else // they just had the same name, not adress..
                found = false;
        } else // Not even the same name
            found = false;

        if(!found) {
            error_logger("Tried removing nonesiting child from node: ", this->getName());
            error_logger("Attempted removal: ", node.getName());
            error_logger("While this node have only the following children");
            for(const auto& str : this->listChildren())
                error_logger("  ", str);

            throw std::runtime_error("Tried removing nonexstiing child from node");
        }

        // removing from childrenRefs is actually O(n), so this operations should not be performed often,
        // especially on nodes with many children.
        // We have to do this before extacting from the ctual store (childrenMap), since that will invalidate
        // the ref to the child that is being removed.
        for(auto it2 = childrenRefs.begin(); it2 != childrenRefs.end(); ++it2)
            if( (*it2).get().getName() == node.getName()) {
                childrenRefs.erase(it2);
                break;
            }
        
        // Transfer ownership:
        extracted_ptr = std::move((*it).second);
        
        // We can now remove this entry from the internal vectors:
        childrenMap.erase(it);

        return extracted_ptr;
    }


    std::vector<std::string> SceneNode::listChildren() const {
        std::vector<std::string> list;
        for(const auto& entity : childrenMap)
            list.push_back(entity.first);
        return list;
    }

    const std::string& SceneNode::getName() const {
        return name;
    }
    
    bool SceneNode::operator==(const SceneNode& other) const {
        if(this->getName() == other.getName() && this == &other)
            return true;
        else
            return false;

    }
    
    bool SceneNode::operator!=(const SceneNode& other) const {
        return !((*this)==other);
    }


    mat4d   SceneNode::getLocalToParent() const{
        return localToParent;
    }

    void    SceneNode::setLocalToParent(const mat4d& m) {
        localToParent = m;
    }

    mat4d   SceneNode::getLocalToWorld() const {
        return localToWorld;
    }
    
    mat4d   SceneNode::getWorldToLocal() const {
        return localToWorld.inverse();
    }

    vec3d   SceneNode::getWorldPos() const {
        return worldPos;
    }

    box3d   SceneNode::getWorldBounds() const {
        return worldBounds;
    }

    void SceneNode::setLocalBounds(const box3d& bounds) {
        localBounds = bounds;
    }

    void SceneNode::enlargeLocalBounds(const box3d& bounds) {
        localBounds = localBounds.enlarge(bounds);
    }


    void   SceneNode::updateLocalToWorld(const mat4d& parentLocalToWorld) {
        localToWorld = parentLocalToWorld*localToParent;

        for(auto& entity : childrenMap)
            entity.second->updateLocalToWorld(localToWorld);

        worldPos = localToWorld * vec3d::ZERO;

        worldBounds = localToWorld * localBounds;
        for(auto& entity : childrenMap)
           worldBounds = worldBounds.enlarge(entity.second->getWorldBounds()); 
    }

    bool SceneNode::isVisible() const {
        return visible;
    }

    void SceneNode::isVisible(bool _visible) {
        visible = _visible;
    }

    void SceneNode::draw(const Program& prog) const {
        if(!isVisible())
            return;

        // No drawing for SceneNode base class...


        for(const SceneNode& child : childrenRefs) {
            child.draw(prog);

        }
    }
}
