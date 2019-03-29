#ifndef _MORK_MODEL_H_
#define _MORK_MODEL_H_

#include <vector>
#include <unordered_map>

#include "mork/render/Mesh.h"
#include "mork/render/Texture.h"
#include "mork/render/Material.h"

#include "mork/scene/SceneNode.h"

namespace mork {
    
    using VTBN = vertex_pos_norm_tang_bitang_uv;

    class Model : public SceneNode {
        public:
            Model(const Model&) = delete;
            Model& operator=(const Model&) = delete;
            Model(Model&&) = default;
            Model& operator=(Model&&) = default;

            Model(const std::string& name);
            Model(const std::string& name, Mesh<VTBN>&& mesh, Material&& material);


            void addMaterial(Material&& mat);
            void addMesh(Mesh<VTBN>&& mesh);

            const std::vector<Material>&  getMaterials() const;
            std::vector<Material>&  getMaterials();
            
            const Mesh<VTBN>& getMesh(unsigned int index) const;
            Mesh<VTBN>& getMesh(unsigned int index);


			virtual void draw(const Program& prog) const;
        private:

            std::vector<Material>       materials;
            std::vector<Mesh<VTBN> >      meshes;
            

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
