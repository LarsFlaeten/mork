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

    void Scene::draw(const Program& prog) {
        for(auto& cam : cameras) {
            computeVisibility(*cam, root, PARTIALLY_VISIBLE);

            // DRAW
            // TODO: Make predicates for drawing in order to be able to do passes

            root.draw(prog);
        }

    }
            
    void Scene::addCamera(std::shared_ptr<Camera> camera) {
        this->cameras.push_back(camera);
    }

    void Scene::computeVisibility(const Camera& cam, SceneNode& node, Visibility v) {
        // Do explicit calc on visibility of this nod if parent is partially visible
        if(v == PARTIALLY_VISIBLE) {
            v = cam.getWorldFrustum().getVisibility(node.getWorldBounds());
        }

        // Set not to visible if partially or fully visible
        node.isVisible( v != INVISIBLE );

        for(auto& child : node.getChildren()) {
            computeVisibility(cam, *child, v);
        }
    }
}
