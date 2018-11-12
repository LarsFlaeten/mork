#include "mork/render/Font.h"
#include "mork/render/FontEngine.h"

namespace mork {

    Font::Font() {
        vao.bind();
        vbo.bind();
        vbo.setBufferSize(sizeof(float)*6*5);
        vbo.setAttributes();
        vbo.unbind();
        vao.unbind();    

    }

    void Font::drawText(const std::string& text, float x, float y, float scale, const vec3f& color) {
        vao.bind();
        FontEngine::getProgram().use();        
        FontEngine::getProgram().getUniform("textColor").set(color);

        glActiveTexture(GL_TEXTURE0);

        for(auto c : text) {
            
            Glyph& ch = *characters[c]; 

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
			ch.texture.bind();
        	// Update content of VBO memory
        	vbo.bind();
			// TODO: make new method in VB class to do this
        	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        	vbo.unbind();
        
			// Render quad
        	glDrawArrays(GL_TRIANGLES, 0, 6);
        	// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        	x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
            ch.texture.unbind();
        }

        vao.unbind();

    }




}
