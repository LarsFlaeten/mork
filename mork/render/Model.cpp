#include "mork/render/Model.h"
#include "mork/resource/ResourceFactory.h"
#include "mork/util/Util.h"

#include <memory>

namespace mork {

    ModelNode::ModelNode() {
       
    }
    
    void ModelNode::addMeshIndex(unsigned int index) {
        meshIndices.push_back(index);
    }

    const std::vector<unsigned int>&  ModelNode::getMeshIndices() const {
        return meshIndices;
    }

    void ModelNode::draw(const Program& prog, const Model& model) const {

        mork::mat4d modelMat = getLocalToWorld();
        mork::mat3d normalMat = ((modelMat.inverse()).transpose()).mat3x3();
        prog.getUniform("model").set(modelMat.cast<float>());
        
        if(prog.queryUniform("normalMat")) {
            prog.getUniform("normalMat").set(normalMat.cast<float>());
        }
        
        for(unsigned int index : getMeshIndices()) {
            auto& mesh = model.getMesh(index);
            // Test for material.ambient color, and assume the whole material structure
            // is present in the shader is this is true:            
            if(prog.queryUniform("material.ambientColor")) {
                const Material& mat = model.getMaterials()[mesh.getMaterialIndex()];
	            mat.set(prog, "material");
                mat.bindTextures();
            }
            if(prog.queryUniform("scale")) {
                // Set to 1=10th of characteristoc size of this object:
                prog.getUniform("scale").set((float)this->getWorldBounds().norm()/10.0f);
            }
            mesh.draw();
            
        }

        for(SceneNode& node : childrenRefs) {
            const auto& modelNode = dynamic_cast<ModelNode&>(node);
            modelNode.draw(prog, model);
        }

    }

    Model::Model(const std::string& name) : SceneNode(name) {

    }
    
    Model::Model(const std::string& name, Mesh<VTBN>&& mesh, Material&& material)
        : SceneNode(name) {

        meshes.push_back(std::move(mesh));
        materials.push_back(std::move(material));
        auto& m = meshes[meshes.size()-1];
        m.setMaterialIndex(materials.size()-1);
        
        std::unique_ptr<ModelNode> node = std::make_unique<ModelNode>();

        node->addMeshIndex(meshes.size()-1);
        node->setLocalBounds(m.getBounds());
        
        this->addChild(std::move(node));
    }

    void Model::addMesh(Mesh<VTBN>&& mesh) {
        meshes.push_back(std::move(mesh));
    }
    
    const Mesh<VTBN>& Model::getMesh(unsigned int index) const {
        return meshes[index];
    }
    
    Mesh<VTBN>& Model::getMesh(unsigned int index){
        return meshes[index];
    }
     
    unsigned int Model::getNumMeshes() const {
        return meshes.size();
    }


    void Model::addMaterial(Material&& material) {
        materials.push_back(std::move(material));
    }

    const std::vector<Material>& Model::getMaterials() const {
        return materials;
    }
     
    std::vector<Material>& Model::getMaterials(){
        return materials;
    }
   
	void Model::draw(const Program& prog) const {

		for(SceneNode& node : childrenRefs) {
            const auto& modelNode = dynamic_cast<ModelNode&>(node); 
            modelNode.draw(prog, *this);

        }
	}

    inline json modelSchema = R"(
    {
        "$schema": "http://json-schema.org/draft-07/schema#",
        "title": "Model schema",
        "type": "object",
        "description": "A model object, derived from sceneNode",
        "properties": {
            "name": { "type": "string" },
            "meshes": { "type": "array", "items": { "type": "object"} },
            "materials": { "type": "array", "items": { "type": "object"} },
            "transform": { "type": "array", "items": { "type": "object" } }
        },
        "additionalProperties": false,
        "required": ["name"]
    }
    )"_json;


    class ModelResource: public ResourceTemplate<std::unique_ptr<Model> >
    {
		public:
		    ModelResource(ResourceManager& manager, Resource& r) :
				ResourceTemplate<std::unique_ptr<Model> >(modelSchema)
			{
				info_logger("Resource - Model");
            	const json& js = r.getDescriptor();
                validator.validate(js);
                
                std::string modelName = js["name"];
                model = std::make_unique<Model>(modelName);

                json meshes = js["meshes"];
                for( auto& arrayObject : meshes) {
                    Resource& mesh_r = r.addChildResource(Resource(manager, "mesh", arrayObject, r.getFilePath()));
                     
                    auto mesh = ResourceFactory<Mesh<VTBN> >::getInstance().create(manager, mesh_r);

                    // Assume key is a mesh:
                    auto bounds = mesh.getBounds();
                    model->addMesh(std::move(mesh));

                    // Add a new model node for this mesh:
                    auto node = std::make_unique<ModelNode>();

                    node->addMeshIndex(model->getNumMeshes()-1);
                    node->setLocalBounds(bounds);
        
                    model->addChild(std::move(node));

                }

                json materials = js["materials"];
                for( auto& arrayObject : materials) {
                    Resource& mat_r = r.addChildResource(Resource(manager, "material", arrayObject, r.getFilePath()));
                    auto material = ResourceFactory<Material>::getInstance().create(manager, mat_r);

                    model->addMaterial(std::move(material));
                }
                
                json tr = js["transform"];
                mat4d trans = mat4d::IDENTITY;
                for( auto& tr_a : tr) {
                    if(tr_a.count("translate")) {
                        std::string vs = tr_a["translate"].get<std::string>();
                        vec3d v = string2vec3d(vs);
                        trans = trans * mat4d::translate(v);                        
                    } else if(tr_a.count("rotatez")) {
                        std::string rzs = tr_a["rotatez"].get<std::string>();
                        double rz = stod(rzs);
                        trans = trans * mat4d::rotatez(radians(rz));

                    } else throw std::runtime_error("should not be here");
                }
                model->setLocalToParent(trans); 
           
		    }

            std::unique_ptr<Model> releaseResource() {
				return std::move(model);

            }
		private:
            std::unique_ptr<Model> model;			

    };

    inline std::string model = "model";

    static ResourceFactory<std::unique_ptr<SceneNode> >::Type<model, ModelResource> ModelType;

   
}
