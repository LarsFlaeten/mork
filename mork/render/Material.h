#ifndef _MORK_MATERIAL_H_
#define _MORK_MATERIAL_H_


#include <vector>

#include <mork/math/vec3.h>
#include <mork/render/Texture.h>
#include <mork/render/Program.h>

namespace mork {
        
    enum Op : unsigned int { ADD = 1, MULTIPLY = 2 };
 
    // Wrapper class for textures that adds an blend op and blend factor
    class TextureLayer {
       
        public:
            TextureLayer();
            TextureLayer(Texture<2>&& texture, Op op, float blendFactor);
            TextureLayer(Texture<2>&& texture);
            TextureLayer(TextureLayer&& o) noexcept;
            TextureLayer& operator=(TextureLayer&& o) noexcept;

            Texture<2>  texture;
            Op          op;
            float       blendFactor;


    };
   

    class Material {
        public:
            Material();
            Material(Material&& o) noexcept;

            void set(const Program& prog, const std::string& target) const;
            void bindTextures() const;


            // Base colors:
            vec3f                   ambientColor;
            vec3f                   diffuseColor;
            vec3f                   specularColor;
            vec3f                   emissiveColor;
            // Other props
            float                   shininess;
            

            // Texture layers
            std::vector<TextureLayer > diffuseLayers;
            std::vector<TextureLayer > specularLayers;
            std::vector<TextureLayer > ambientLayers;
            std::vector<TextureLayer > emissiveLayers;

            // Other layers
            std::vector<TextureLayer> normalLayers;
            std::vector<TextureLayer> heightLayers;



    };
}
#endif

