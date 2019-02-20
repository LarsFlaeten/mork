#ifndef _MORK_MODEL_H_
#define _MORK_MODEL_H_

#include <vector>
#include <unordered_map>

#include "mork/render/Mesh.h"
#include "mork/render/Texture.h"
#include "mork/render/Material.h"

#include "mork/scene/SceneNode.h"

namespace mork {
    


    class Model : public SceneNode {
        public:
            Model(const std::string& name);

            void addMaterial(Material&& mat);
            void addMesh(BasicMesh&& mesh);

            const std::vector<Material>&  getMaterials() const;
            std::vector<Material>&  getMaterials();
            
            const BasicMesh& getMesh(unsigned int index) const;

			virtual void draw(const Program& prog) const;
        private:

            std::vector<Material>       materials;
            std::vector<BasicMesh>      meshes;
            

    };

    class ModelNode : public SceneNode {
        public:
            ModelNode();

            const std::vector<unsigned int>& getMeshIndices() const;
            
            void addMeshIndex(unsigned int index);
    
            virtual void draw(const Program& prog, const Model& model) const; 
        protected:
            std::vector<unsigned int>    meshIndices;

    };



}

#endif
