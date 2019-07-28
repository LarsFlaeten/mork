#include "mork/scene/Scene.h"
#include "mork/resource/ResourceFactory.h"
#include "mork/util/Util.h"

namespace mork {

    Scene::Scene() : root("root") {}

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

    
        // When all nodes are updated, we can update the camera
        // relative to its target node (if any)
        camera.update();
        
        // We can no compute visibility
        // This call is moved from the draw method to the end of update as of 04.07.2019
        // This is because exteranl applications may not use the stanfdard draw method,
        // but needs the visibility to be calculated
        computeVisibility(camera, root, PARTIALLY_VISIBLE);

   }

    void Scene::draw(const Program& prog) {
        // DRAW
        // TODO: Make predicates for drawing in order to be able to do passes
        prog.use();
        mork::mat4d view = camera.getViewMatrix();
        mork::mat4d proj = camera.getProjectionMatrix(); 
        
        prog.getUniform("projection").set(proj.cast<float>());
        prog.getUniform("view").set(view.cast<float>());
        // Viewpos is not present in all shaders, so check before setting it:
        if(prog.queryUniform("viewPos"))
            prog.getUniform("viewPos").set(camera.getLocalToWorld().translation().cast<float>());


        root.draw(prog);

    }

    void Scene::computeVisibility(const Camera& cam, SceneNode& node, Visibility v) {
        // Do explicit calc on visibility of this nod if parent is partially visible
        if(v == PARTIALLY_VISIBLE) {
            v = cam.getWorldFrustum().getVisibility(node.getWorldBounds());
        }

        // Set not to visible if partially or fully visible
        node.isVisible( v != INVISIBLE );

        for(auto& child : node.getChildren()) {
            computeVisibility(cam, child, v);
        }
    }

    inline json sceneSchema = R"(
    {
        "$schema": "http://json-schema.org/draft-07/schema#",
        "title": "Scene schema",
        "type": "object",
        "description": "A scene object",
        "properties": {
            "name": { "type": "string" },
            "rootNode": {
                "type": "object",
                "properties": {
                    "name": { "type": "string", "pattern": "^(root)$" },
                    "childNodes": {
                        "type": "array",
                        "items": [
                            { 
                                "type": "object",
                                "properties": {
                                    "name": { "type": "string" }
                                } 
                            }
                        ]
                    }
                },
                "additionalProperties": false,
                "required": ["name"] 
            },
            "camera": { "type": "object" }
        },
        "additionalProperties": false,
        "required": ["name"]
    }
    )"_json;


    class SceneResource: public ResourceTemplate<Scene>
    {
		public:
		    SceneResource(ResourceManager& manager, Resource& r) :
				ResourceTemplate<Scene>(sceneSchema), scene()
			{
		        info_logger("Resource - Scene");
            	const json& js = r.getDescriptor();
                validator.validate(js);

				if(js.count("rootNode")) {
					json root = js["rootNode"];	
                	// Root node is implicity created by scene. So we add any children
                    // to the scenes' root node:
                    for(auto j : root["childNodes"]) {
                        for( auto& [type, value] : j.items() ) {
                            // We require that the node has a name property (see schema)
                            const std::string& name = value["name"];
                            Resource& node_r = r.addChildResource(Resource(manager, type, value, r.getFilePath()));
                            
                            // Assume key is a Node or a derived class:
                            auto sceneNodePtr = ResourceFactory<std::unique_ptr<SceneNode> >::getInstance().create(manager, node_r);
                            scene.getRoot().addChild(std::move(sceneNodePtr));
                        }

                    }
                    

				}
                
                // TODO: Move all this to camera class?
                if(js.count("camera")) {
                    auto& cam = scene.getCamera();
                    json camj = js["camera"];
                    
                    if(camj.count("reference")) {
                        auto ref = camj["reference"].get<std::string>();
                        auto& n = scene.getRoot().getChild(ref);
                        cam.setReference(n);
                    }

                    Camera::Mode mode = Camera::Mode::FREE;
                    if(camj.count("mode")) {
                        auto ms = camj["mode"].get<std::string>();
                        if(ms.compare("orbit")==0)
                            mode = Camera::Mode::ORBIT;
                    }
                    cam.setMode(mode);


                    if(camj.count("distance")) {
                        auto d = camj["distance"].get<double>();
                        cam.setDistance(d);
                    }
                     
                    if(camj.count("azimuth")) {
                        auto d = camj["azimuth"].get<double>();
                        cam.setAzimuth(d);
                    }
                    
                    if(camj.count("elevation")) {
                        auto d = camj["elevation"].get<double>();
                        cam.setElevation(d);
                    }
                    
                    

                    if(camj.count("position"))
                        cam.setPosition(string2vec3d(camj["position"]));
                    if(camj.count("lookAt")) {
                        json lookAt = camj["lookAt"];
                        vec3d direction = string2vec3d(lookAt["direction"]);
                        vec3d up = string2vec3d(lookAt["up"]);
                        cam.lookAt(direction, up);
                    }
                    


                    double farClip = cam.getFarClippingPlane();
                    double nearClip = cam.getNearClippingPlane();
                    if(camj.count("farClippingPlane"))
                        farClip = camj["farClippingPlane"].get<double>();
                    if(camj.count("nearClippingPlane"))
                        nearClip = camj["nearClippingPlane"].get<double>();
                    cam.setClippingPlanes(nearClip, farClip);

                }
			}

            Scene releaseResource() {
				return std::move(scene);

            }
		private:
			Scene scene;			

    };

    inline std::string scene = "scene";

    static ResourceFactory<Scene>::Type<scene, SceneResource> SceneType;


}
