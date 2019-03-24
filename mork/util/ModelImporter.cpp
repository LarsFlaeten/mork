#include "mork/util/ModelImporter.h"

#include "mork/core/Log.h"
#include "mork/math/vec3.h"
#include "mork/render/VertexBuffer.h"
#include "mork/render/Material.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>

namespace mork {
   
    namespace ModelImporterInternal { 
       

        template<typename Vertex>
        Mesh<Vertex> processMesh(const aiMesh* imesh, const aiScene* scene) {
        }

        template<>
        Mesh<vertex_pos_norm_tang_bitang_uv> processMesh(const aiMesh* imesh, const aiScene* scene) {


            std::vector<vertex_pos_norm_tang_bitang_uv> vertices;
			std::vector<unsigned int> indices;
            for(unsigned int i = 0; i < imesh->mNumVertices; i++)
			{
				// process vertex positions, normals and texture coordinates
				vec3f pos; 
				pos.x = imesh->mVertices[i].x;
				pos.y = imesh->mVertices[i].y;
				pos.z = imesh->mVertices[i].z; 
				
                vec3f norm;
				norm.x = imesh->mNormals[i].x;
                norm.y = imesh->mNormals[i].y;
                norm.z = imesh->mNormals[i].z;

                vec2f uv;

                if(imesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
                {
                    uv.x = imesh->mTextureCoords[0][i].x; 
                    uv.y = imesh->mTextureCoords[0][i].y;
                }
                else {
                    warn_logger("Vertex without uv found, index ", i);
                    uv = vec2f(0.0f, 0.0f);  
                }
                // TODO: Calculate or fetch tangents from assimp
                vertices.push_back(vertex_pos_norm_tang_bitang_uv(pos, norm, vec3f::ZERO, vec3f::ZERO, uv));
			}

            // Process indices:
            for(unsigned int i = 0; i < imesh->mNumFaces; i++)
			{
    			aiFace& face = imesh->mFaces[i];
    			if(face.mNumIndices!=3) {
                    warn_logger("Expected 3 indices per face, found ", face.mNumIndices, " for face ", i);
                } else {
                    for(unsigned int j = 0; j < face.mNumIndices; j++)
        			    indices.push_back(face.mIndices[j]);
                }
			}  

            unsigned int materialIndex = imesh->mMaterialIndex;
               
            return Mesh(vertices, indices, materialIndex); 
			
        }
        
        
        void processNode(const aiNode* inode, const aiScene* iscene, const Model& model, SceneNode& parent) {
            
            std::unique_ptr<ModelNode> node = std::make_unique<ModelNode>();

            // Set initial localbounds with the first mesh
            // This is to handle a case where all meshes are offset from zero and theire
            // local bounds does not include 0,0,0. If only enlarge is applied with a default box of 0,0,0
            // the bounding box will include 0,0,0 which is not what we want
            if(inode->mNumMeshes>0)
                node->setLocalBounds(model.getMesh(inode->mMeshes[0]).getBounds());

            // Process meshes:
            for(int i = 0; i < inode->mNumMeshes; ++i) {
                // Set the model mesh index:
                node->addMeshIndex(inode->mMeshes[i]);
                
                // Add bounds
                node->enlargeLocalBounds(model.getMesh(inode->mMeshes[i]).getBounds());
            }
            
            // Get the transformation to parent:
            // This aiMatrix4x4 uses same storage as mork::Mat4<>,
            // so its basically just to copy tje individual fields: 
            aiMatrix4x4 m = inode->mTransformation;
            mat4d m2 = mat4f(m.a1, m.a2, m.a3, m.a4,
                        m.b1, m.b2, m.b3, m.b4,
                        m.c1, m.c2, m.c3, m.c4,
                        m.d1, m.d2, m.d3, m.d4).cast<double>();
            node->setLocalToParent(m2);
 
            // Add this node to its parent:
            SceneNode& ref = parent.addChild(std::move(node));

            // then do the same for each of its children
            for(unsigned int i = 0; i < inode->mNumChildren; i++)
            {
                processNode(inode->mChildren[i], iscene, model, ref);
            }
        
        }

        Op translateOp(aiTextureOp iop) {
            switch (iop) {
                case(aiTextureOp_Add):
                    return Op::ADD;
                case(aiTextureOp_Multiply):
                    return Op::MULTIPLY;
                default:
                    std::stringstream error;
                    error << "OP: ";
                    error << iop;
                    error << " not supported";
                    error_logger("Translation failed, error was: ", error.str());
                    throw std::runtime_error(error.str());


            }

        }

        std::vector<TextureLayer> getTextureLayers(const aiMaterial* mat, aiTextureType type, const std::string& basePath, Material& material) {
            debug_logger("TextureLayers loader, Num Textures for type", type, ", : ", mat->GetTextureCount(type));
            
            std::vector<TextureLayer> layers;

            int tc = mat->GetTextureCount(type);
            
            for(int j = 0; j < tc; ++j) {
                    
                aiString path;
                aiTextureMapping mapping;
                unsigned int uvIndex;
               aiTextureMapMode mapMode;
                // We skip reading the other properties to the right of path in the following function.
                // We read these manually below wince thay are not allways present
                if( aiReturn_SUCCESS != mat->GetTexture(aiTextureType_DIFFUSE, j, &path)) {
                    error_logger("Could not read type=", type, ", tecture=", j, " of ", tc);
                    throw std::runtime_error("Error reading texture");
                }

                // Try to fetch these, initialized values will be kept if they do not exist in the texture
                // We set default behaviour to multiply the texture values with the base color
                float blendFactor = 1.0;
                mat->Get(AI_MATKEY_TEXBLEND(type, j), blendFactor);
                
                aiTextureOp iop = aiTextureOp_Multiply;
                mat->Get(AI_MATKEY_TEXOP(type, j), iop);

                // TODO: Get texture warp modes and other relevant stuff 

                Op op = translateOp(iop);
                Texture<2> texture;
                texture.loadTexture(std::string(basePath + path.C_Str()), true);

                layers.push_back(TextureLayer(std::move(texture), op, blendFactor));
            }

            return layers;
        }

        std::string getMatProperties(const aiMaterial* mat) {
            std::stringstream ss;

            ss << "Num properties: " << mat->mNumProperties << "\n";
            for(int i = 0; i < mat->mNumProperties; ++i) {
                const aiMaterialProperty* prop = mat->mProperties[i];

                ss << "Property " << i << "\n";
                ss << "mKey:        " << prop->mKey.C_Str() << "\n";
                ss << "mType:       " << prop->mType << "\n";
                ss << "mIndex:      " << prop->mIndex << "\n";
                ss << "mSemantic:   " << prop->mSemantic << "\n";
                ss << "mDataLength: " << prop->mDataLength << "\n";
                ss << "mData        " << prop->mData << "\n";
            }
            return ss.str();


        }
        
        void loadMeshes(const aiScene* iscene, Model& model){
            debug_logger("Num Meshes: ", iscene->mNumMeshes);
            for(int i = 0; i < iscene->mNumMeshes; ++i) {
                const aiMesh* imesh = iscene->mMeshes[i];
                auto mesh = processMesh<vertex_pos_norm_tang_bitang_uv>(imesh, iscene);
                model.addMesh(std::move(mesh));


            }


        }
 
        void loadMaterials(const aiScene* scene, Model& model, const std::string& basePath){
            debug_logger("Num Materials: ", scene->mNumMaterials);
            for(int i = 0; i < scene->mNumMaterials; ++i)
            {
                // The material we are extracting from
                const aiMaterial* mat = scene->mMaterials[i];
              
                // The mork material we are building:
                mork::Material material;
                

                // Get base colors (if defined). Default values are untouched if the material does not contai them
                aiColor3D color (0.f,0.f,0.f);

                color = aiColor3D(0.f, 0.f, 0.f);
                mat->Get(AI_MATKEY_COLOR_AMBIENT,color);
                material.ambientColor = (vec3f(color.r, color.b, color.g));

                color = aiColor3D(0.f, 0.f, 0.f);
                mat->Get(AI_MATKEY_COLOR_DIFFUSE,color);
                material.diffuseColor = (vec3f(color.r, color.b, color.g));

                color = aiColor3D(0.f, 0.f, 0.f);
                mat->Get(AI_MATKEY_COLOR_SPECULAR,color);
                material.specularColor = (vec3f(color.r, color.b, color.g));
                
                color = aiColor3D(0.f, 0.f, 0.f);
                mat->Get(AI_MATKEY_COLOR_EMISSIVE,color);
                material.emissiveColor = (vec3f(color.r, color.b, color.g));


                float shininess = 32.0f;
                mat->Get(AI_MATKEY_SHININESS, shininess);
                material.shininess = shininess;

                material.diffuseLayers = std::move(getTextureLayers(mat, aiTextureType_DIFFUSE, basePath, material));

                material.specularLayers = std::move(getTextureLayers(mat, aiTextureType_SPECULAR, basePath, material));

                material.ambientLayers = std::move(getTextureLayers(mat, aiTextureType_AMBIENT, basePath, material));

                material.emissiveLayers = std::move(getTextureLayers(mat, aiTextureType_EMISSIVE, basePath, material));

                material.normalLayers = std::move(getTextureLayers(mat, aiTextureType_NORMALS, basePath, material));

                material.heightLayers = std::move(getTextureLayers(mat, aiTextureType_HEIGHT, basePath, material));


                model.addMaterial(std::move(material));

               
           }

        }
        

    }

    Model ModelImporter::loadModel(const std::string& path, const std::string& file, const std::string& nodeName) {

        Assimp::Importer importer;
        std::string filepath = path + file;

        const aiScene *scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_GenUVCoords );

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
        {
            error_logger("ModelImporter:\n", importer.GetErrorString());
            throw std::runtime_error("Failed loading model");
        }

        Model model(nodeName);

        // Load common materials:
        ModelImporterInternal::loadMaterials(scene, model, path);

        // Load all meshes
        ModelImporterInternal::loadMeshes(scene, model);
        
        // Create the node tree
        ModelImporterInternal::processNode(scene->mRootNode, scene, model, model);

        return model;
    }

    
}
