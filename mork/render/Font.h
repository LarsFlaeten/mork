#ifndef _MORK_FONT_H_
#define _MORK_FONT_H_

#include <map>
#include <memory>

#include "mork/render/Texture.h"
#include "mork/math/vec2.h"
#include "mork/math/vec3.h"

#include "mork/render/VertexArrayObject.h"
#include "mork/render/VertexBuffer.h"

namespace mork {


    struct Glyph {

        Glyph() {

        }

        Glyph(Glyph&& o) :
            texture(std::move(o.texture)),
            size(o.size),
            bearing(o.bearing),
            advance(o.advance)
            { }


        Texture<2>   texture;
        vec2i       size;
        vec2i       bearing;
        unsigned int    advance;
    };

    struct Font {
        Font();

        

        void    drawText(const std::string& text, float x, float y, float scale, const vec3f& color);
        
    


        std::map<char, std::shared_ptr<Glyph> > characters;

        private:
        VertexArrayObject   vao;
        DynamicVertexBuffer<mork::vertex_pos_uv> vbo;
 
    };


}


#endif
