#include "mork/render/Material.h"
#include "mork/resource/ResourceFactory.h"
#include "mork/util/Util.h"

namespace mork {

    TextureLayer::TextureLayer()
        : blendFactor(1.0f), op(ADD), texture(){} 

    TextureLayer::TextureLayer(Texture<2>&& texture, Op op, float blendFactor)
        : texture(std::move(texture)), op(op), blendFactor(blendFactor) {}

    TextureLayer::TextureLayer(Texture<2>&& texture)
        : texture(std::move(texture)), op(Op::ADD), blendFactor(1.0f) {}


    
    TextureLayer::TextureLayer(TextureLayer&& o) noexcept :
        texture(std::move(o.texture)),
        op(o.op),
        blendFactor(o.blendFactor)
    {
    }

    TextureLayer& TextureLayer::operator=(TextureLayer&& o) noexcept {
        if(o.texture != texture) {
            texture = std::move(o.texture);
            op = o.op;
            blendFactor = o.blendFactor;
        }

        return *this;

    }

    // All values inititialized to default
    Material::Material() :
        ambientColor(vec3f::ZERO),
        diffuseColor(vec3f::ZERO),
        specularColor(vec3f::ZERO),
        emissiveColor(vec3f::ZERO),
        shininess(32.0f),
        reflectiveFactor(0.0f),
        refractiveFactor(0.0f),            
        refractiveIndex(0.0f) {}

/*    Material::Material(Material&& o) noexcept
        : diffuseLayers(std::move(o.diffuseLayers)),
          specularLayers(std::move(o.specularLayers)),
          ambientLayers(std::move(o.ambientLayers)),
          emissiveLayers(std::move(o.emissiveLayers)),
          normalLayers(std::move(o.normalLayers)),
          heightLayers(std::move(o.heightLayers)),
          shininess(o.shininess),
          reflectiveFactor(o.reflectiveFactor),
          refractiveFactor(o.refractiveFactor),            
          refractiveIndex(o.refractiveIndex),          
          ambientColor(o.ambientColor),
          diffuseColor(o.diffuseColor),
          specularColor(o.specularColor),
          emissiveColor(o.emissiveColor){ }
  */  
    void Material::set(const Program& prog, const std::string& target) const {
        // Set base colors:        
        prog.getUniform(target + ".ambientColor").set(ambientColor);
        prog.getUniform(target + ".diffuseColor").set(diffuseColor);
        prog.getUniform(target + ".specularColor").set(specularColor);
        prog.getUniform(target + ".emissiveColor").set(emissiveColor);

        prog.getUniform(target + ".shininess").set(shininess);        
        
        prog.getUniform(target + ".reflectiveFactor").set(reflectiveFactor);
        prog.getUniform(target + ".refractiveFactor").set(refractiveFactor);
        prog.getUniform(target + ".refractiveIndex").set(refractiveIndex);

        // Running counter for active textures
        int tex = 0;

        unsigned int l = 0;
       
        l = ambientLayers.size();
        prog.getUniform(target + ".numAmbientLayers").set(l);
        for(int i = 0; i < l; ++i) {
            std::string num = std::to_string(i);
            prog.getUniform(target + ".ambientLayers[" + num + "].texture").set(tex++);
            prog.getUniform(target + ".ambientLayers[" + num + "].op").set(ambientLayers[i].op);
            prog.getUniform(target + ".ambientLayers[" + num + "].blendFactor").set(ambientLayers[i].blendFactor);
        }

        l = diffuseLayers.size();
        prog.getUniform(target + ".numDiffuseLayers").set(l);
        for(int i = 0; i < l; ++i) {
            std::string num = std::to_string(i);
            prog.getUniform(target + ".diffuseLayers[" + num + "].texture").set(tex++);
            prog.getUniform(target + ".diffuseLayers[" + num + "].op").set(diffuseLayers[i].op);
            prog.getUniform(target + ".diffuseLayers[" + num + "].blendFactor").set(diffuseLayers[i].blendFactor);
        }

        l = specularLayers.size();
        prog.getUniform(target + ".numSpecularLayers").set(l);
        for(int i = 0; i < l; ++i) {
            std::string num = std::to_string(i);
            prog.getUniform(target + ".specularLayers[" + num + "].texture").set(tex++);
            prog.getUniform(target + ".specularLayers[" + num + "].op").set(specularLayers[i].op);
            prog.getUniform(target + ".specularLayers[" + num + "].blendFactor").set(specularLayers[i].blendFactor);
        }

        l = emissiveLayers.size();
        prog.getUniform(target + ".numEmissiveLayers").set(l);
        for(int i = 0; i < l; ++i) {
            std::string num = std::to_string(i);
            prog.getUniform(target + ".emissiveLayers[" + num + "].texture").set(tex++);
            prog.getUniform(target + ".emissiveLayers[" + num + "].op").set(emissiveLayers[i].op);
            prog.getUniform(target + ".emissiveLayers[" + num + "].blendFactor").set(emissiveLayers[i].blendFactor);
        }
        
        l = normalLayers.size();
        prog.getUniform(target + ".numNormalLayers").set(l);
        for(int i = 0; i < l; ++i) {
            std::string num = std::to_string(i);
            prog.getUniform(target + ".normalLayers[" + num + "].texture").set(tex++);
            prog.getUniform(target + ".normalLayers[" + num + "].op").set(normalLayers[i].op);
            prog.getUniform(target + ".normalLayers[" + num + "].blendFactor").set(normalLayers[i].blendFactor);
        }


    }

    void Material::bindTextures() const {
        // Running counter for active textures
        int tex = 0;
       
        unsigned int l = 0;
       
        l = ambientLayers.size();
        for(int i = 0; i < l; ++i) {
            ambientLayers[i].texture.bind(tex++);
        }

        l = diffuseLayers.size();
        for(int i = 0; i < l; ++i) {
            diffuseLayers[i].texture.bind(tex++);
        }

        l = specularLayers.size();
        for(int i = 0; i < l; ++i) {
            specularLayers[i].texture.bind(tex++);
        }

        l = emissiveLayers.size();
        for(int i = 0; i < l; ++i) {
            emissiveLayers[i].texture.bind(tex++);
        }
        
        l = normalLayers.size();
        for(int i = 0; i < l; ++i) {
            normalLayers[i].texture.bind(tex++);
        }


    }

    inline json materialSchema = R"(
    {
        "$schema": "http://json-schema.org/draft-07/schema#",
        "title": "Model schema",
        "type": "object",
        "description": "A material object",
        "properties": {
            "name": { "type": "string" },
            "diffuseColor": { "type": "string" },
            "diffuseLayers": { "type": "array", "items": { "type": "object"} },
            "normalLayers": { "type": "array", "items": { "type": "object" } }
        },
        "additionalProperties": false,
        "required": ["name"]
    }
    )"_json;


    class MaterialResource: public ResourceTemplate<Material>
    {
		public:
		    MaterialResource(ResourceManager& manager, Resource& r) :
				ResourceTemplate<Material>(materialSchema)
			{
			    info_logger("Resource - Material");
         	    const json& js = r.getDescriptor();
                validator.validate(js);
              
                std::cout << js << std::endl;

                std::string materialName = js["name"];

                if(js.count("diffuseColor")) {
                    json dcj = js["diffuseColor"];
                    vec3d dc = string2vec3d(dcj.get<std::string>());
                    material.diffuseColor = dc.cast<float>();
                }

                if(js.count("diffuseLayers")) {
                    json diff = js["diffuseLayers"];
                    for( auto& arrayObject : diff) {
                        // This is a texture layer object, with a texture2d, an op and a blend factor
                        Op op = Op::ADD;
                        float bf = 1.0f;
                       
                        // TODO: get op and bf

                        json texj = arrayObject["texture2d"];
                        Resource& cr = r.addChildResource(Resource(manager, "texture2d", texj, texj["file"]));
    
                        auto tex = ResourceFactory<Texture<2>>::getInstance().create(manager, cr);
                        
                        material.diffuseLayers.push_back(TextureLayer(std::move(tex), op, bf)); 
                    }
                }
                
                if(js.count("normalLayers")) {
                    json norm = js["normalLayers"];
                    for( auto& arrayObject : norm) {
                        // This is a texture layer object, with a texture2d, an op and a blend factor
                        Op op = Op::ADD;
                        float bf = 1.0f;
                         // TODO: get op and bf

                       
                        json texj = arrayObject["texture2d"];
                         
                        Resource& cr = r.addChildResource(Resource(manager, "texture2d", texj, texj["file"]));
    
                        auto tex = ResourceFactory<Texture<2>>::getInstance().create(manager, cr);
     
                        material.normalLayers.push_back(TextureLayer(std::move(tex), op, bf)); 
                    }
                }
                // TODO: get other layers




            }

            Material releaseResource() {
				return std::move(material);

            }
		private:
            Material material;			

    };

    inline std::string material = "material";

    static ResourceFactory<Material>::Type<material, MaterialResource> MaterialType;



}
