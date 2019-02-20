#ifndef _MORK_FONT_H_
#define _MORK_FONT_H_

#include <map>
#include <memory>

#include "mork/render/Texture.h"
#include "mork/math/vec2.h"
#include "mork/math/vec3.h"

#include "mork/render/VertexArrayObject.h"
#include "mork/render/VertexBuffer.h"
#include "mork/render/Program.h"

namespace mork {


    struct Font {
        Font();

        // size = size in pixels of the rendered font
        void    drawText(const std::string& text, float x, float y, float size,
                const vec3f& color, const mat4f& projection) const;
        
        // size = size in pixels to generate font textures. Larger number gives
        //          improved resoluiont with large fonts at rendertime, but increases
        //          texture usage
        static  Font createFont(const std::string& ttfPath, unsigned int size = 48);


        // returns the maximum font heigh above baselin for the given font size
        int getYMax(float fontSize) const;
        
        // returns the minimum font heigh (below the baseline) for the given font size
        int getYMin(float fontSize) const;

        private:
        struct Glyph {

            Glyph() {

            }

            Glyph(Glyph&& o) :
                texture(std::move(o.texture)),
                size(o.size),
                bearing(o.bearing),
                advance(o.advance)
                {
                    o.size = vec2i::ZERO;
                    o.bearing = vec2i::ZERO;
                    o.advance = 0;
                }


            Texture<2>   texture;
            vec2i       size;
            vec2i       bearing;
            unsigned int    advance;
        };

        Program             prog;
        VertexArrayObject   vao;
        DynamicVertexBuffer<mork::vertex_pos_uv> vbo;
        std::map<char, Glyph > characters;
        float yMax, yMin, yAdvance;

        static std::string font_vs;
        static std::string font_ps;
 
        float loadedSize;
    };


}


#endif
