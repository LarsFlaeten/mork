#ifndef _MORK_MODELIMPORTER_H_
#define _MORK_MODELIMPORTER_H_

#include <string>

#include "mork/render/Model.h"


namespace mork {

    class ModelImporter {
        public:
            static std::shared_ptr<Model>   loadModel(const std::string& path, const std::string& file);
            //static void processNode(const aiNode* node, const aiScene* scene, Model& model);
            //static BasicMesh processMesh(const aiMesh* aimesh, const aiScene* scene);
 

    };



}

#endif
