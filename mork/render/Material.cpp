#include "mork/render/Material.h"


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

}
