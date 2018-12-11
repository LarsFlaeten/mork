#include "mork/render/Font.h"

#include <freetype/include/ft2build.h>
#include FT_FREETYPE_H


namespace mork {

    std::string Font::font_vs = 
R"(
#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;
out vec2 TexCoords;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(pos.xy, 0.0, 1.0);
    TexCoords = tex;
}
)";  

	std::string Font::font_ps = 
R"(
#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    color = vec4(textColor, 1.0) * sampled;
}
)";   
 
    Font::Font() 
        : prog(font_vs, font_ps){
        vao.bind();
        vbo.bind();
        vbo.setBufferSize(sizeof(float)*6*5);
        vbo.setAttributes();
        vbo.unbind();
        vao.unbind();    

        yMax = 0.0f;
        yMin = 0.0f;
        yAdvance = 0.0f;

    }
    Font Font::createFont(const std::string& ttfPath, unsigned int size ) {
        if(size < 4)
            throw std::runtime_error("Size < 4 not allowed when creating fonts");
        Font font;
        font.loadedSize = size;


        FT_Library ft;
        if (FT_Init_FreeType(&ft)) {
            throw std::runtime_error("FREETYPE: Could not init FreeType Library");
        }
 
        FT_Face face;
              
        if (FT_New_Face(ft, ttfPath.c_str(), 0, &face)) {
            error_logger("Error loading font \"", ttfPath, "\"..");
            throw std::runtime_error("FREETYPE: Failed to load font");
        }
 
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

        FT_Set_Pixel_Sizes(face, 0, size); 

        // Load the standard characters
        for(unsigned char c = 0; c < 128; ++c) {
            Glyph glyph;

            // Load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                warn_logger("ERROR::FREETYTPE: Failed to load Glyph, font: ", ttfPath, ", character: [", c, "].. Font will be skipped");
                continue;
            }                

            glyph.texture.loadTexture(
                    face->glyph->bitmap.width,
                    face->glyph->bitmap.rows,
                    1, // Only one channel, freetype is monochrome
                    face->glyph->bitmap.buffer);

            glyph.size = vec2i(face->glyph->bitmap.width, face->glyph->bitmap.rows);
            glyph.bearing = vec2i(face->glyph->bitmap_left, face->glyph->bitmap_top);
            glyph.advance = face->glyph->advance.x;

            if(font.yMax < glyph.bearing.y)
                font.yMax = glyph.bearing.y;
            if(font.yMin > -(glyph.size.y - glyph.bearing.y))
                font.yMin = -(glyph.size.y - glyph.bearing.y);
             
            font.characters.insert(std::make_pair(static_cast<char>(c), std::move(glyph)));

        }
        font.yAdvance = font.yMax - font.yMin; 
    
        // Cleanup freetype
        FT_Done_Face(face);
        FT_Done_FreeType(ft);


        // will invoke std::move
        return font;

    }


    void Font::drawText(const std::string& text, float x, float y, float size, const vec3f& color, const mat4f& projection) {
        vao.bind();
        prog.use();        
        prog.getUniform("textColor").set(color);
        prog.getUniform("projection").set(projection);
        prog.getUniform("text").set(0); // Texture unit
 
        float base_x = x;
        float base_y = y;

        float scale = size/loadedSize;

        for(auto c : text) {
            // Handle Carriage return and tab
            if(c == '\n') {
                x = base_x;
                base_y = base_y - this->yAdvance*scale;
                y = base_y;
                continue;
            } else if(c == '\t') {
                // TODO: Configure tab behavoiur? 4 spaces for now:
                const Glyph& sp = characters[32];
                x += 4*(sp.advance >> 6)*scale;
                continue;
            }
            const Glyph& ch = characters[c]; 

            float xpos = x + ch.bearing.x * scale;
            float ypos = y - (ch.size.y - ch.bearing.y) * scale;

            float w = ch.size.x * scale;
            float h = ch.size.y * scale;

            float vertices[6][5] = {
            	{ xpos,     ypos + h,   0.0, 0.0, 0.0 },            
            	{ xpos,     ypos,       0.0, 0.0, 1.0 },
            	{ xpos + w, ypos,       0.0, 1.0, 1.0 },

            	{ xpos,     ypos + h,   0.0, 0.0, 0.0 },
            	{ xpos + w, ypos,       0.0, 1.0, 1.0 },
            	{ xpos + w, ypos + h,   0.0, 1.0, 0.0 }           
        	};

            // Render glyph texture over quad
			ch.texture.bind(0);
        	// Update content of VBO memory
        	vbo.bind();
			// TODO: make new method in VB class to do this
        	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        	vbo.unbind();
        
			// Render quad
        	glDrawArrays(GL_TRIANGLES, 0, 6);
        	// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        	x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
            ch.texture.unbind(0);
        }

        vao.unbind();

    }




}
