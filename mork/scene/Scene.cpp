#include "mork/scene/Scene.h"

namespace mork {

    Scene::Scene() {
    
    }

    Scene::~Scene() {

    }

    const SceneNode& Scene::getRoot() const {
        return root;
    }

    SceneNode& Scene::getRoot() {
        return root;
    }

    void Scene::update() {

        // Traverse the node tree from root and up
        // We give identity as the first mapping for root nodes "parent"
        root.updateLocalToWorld(mat4d::IDENTITY);

    
        // When all nodes are updated, we can update all cameras
        // relative to their target nodes (if they have any)
        for(auto& cam : cameras) {
            cam->update();
        }

    }
            
    void Scene::addCamera(std::shared_ptr<Camera> camera) {
        this->cameras.push_back(camera);
    }


}
