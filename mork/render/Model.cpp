#include "mork/render/Model.h"

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
        prog.getUniform("normalMat").set(normalMat.cast<float>());
        
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
    
    Model::Model(const std::string& name, BasicMesh&& mesh, Material&& material)
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

    void Model::addMesh(BasicMesh&& mesh) {
        meshes.push_back(std::move(mesh));
    }
    
    const BasicMesh& Model::getMesh(unsigned int index) const {
        return meshes[index];
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

    
}
