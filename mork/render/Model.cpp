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
        for(unsigned int index : getMeshIndices()) {
            auto& mesh = model.getMesh(index);

            const Material& mat = model.getMaterials()[mesh.getMaterialIndex()];
	        mat.set(prog, "material");
            mat.bindTextures();
            mesh.draw();
            
        }

        for(auto child : children) {
            auto modelNode = std::dynamic_pointer_cast<ModelNode>(child);
            modelNode->draw(prog, model);
        }

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

		for(auto sceneNode : children) {
            auto modelNode = std::dynamic_pointer_cast<ModelNode>(sceneNode);
            modelNode->draw(prog, *this);

        }
	}

    
}
