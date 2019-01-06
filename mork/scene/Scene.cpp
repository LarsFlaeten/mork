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

    const Camera& Scene::getCamera() const {
        return camera;
    }

    Camera& Scene::getCamera() {
        return camera;
    }


    void Scene::update() {

        // Traverse the node tree from root and up
        // We give identity as the first mapping for root nodes "parent"
        root.updateLocalToWorld(mat4d::IDENTITY);

    
        // When all nodes are updated, we can the camera
        // relative to its target nodes (if any)
        camera.update();

    }

}
