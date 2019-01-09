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

            const Material& mat = model.getMaterials()[mesh.getMaterialIndex()];
	        mat.set(prog, "material");
            mat.bindTextures();
            mesh.draw();
            
        }

        for(SceneNode& node : childrenRefs) {
            const auto& modelNode = dynamic_cast<ModelNode&>(node);
            modelNode.draw(prog, model);
        }

    }

    Model::Model(const std::string& name) : SceneNode(name) {

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
   
	void Model::draw(const Program& prog) const {

		for(SceneNode& node : childrenRefs) {
            const auto& modelNode = dynamic_cast<ModelNode&>(node); 
            modelNode.draw(prog, *this);

        }
	}

    
}
