#include "mork/scene/SceneNode.h"
#include "mork/core/Log.h"

namespace mork {

    SceneNode::SceneNode() 
        : localToParent(mat4d::IDENTITY) {
    }
   
    void SceneNode::addChild(std::shared_ptr<SceneNode> child) {
        children.push_back(child);
    }

    void SceneNode::addChild(SceneNode&& child) {
        children.push_back(std::make_shared<SceneNode>(std::move(child)));
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


    void   SceneNode::setLocalToWorld(const mat4d& parentLocalToWorld) {
        localToWorld = parentLocalToWorld*localToParent;

        for(auto& child : children)
            child->setLocalToWorld(localToWorld);
    }







}
