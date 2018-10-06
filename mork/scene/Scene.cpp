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
        root.setLocalToWorld(mat4d::IDENTITY);

    }

}
