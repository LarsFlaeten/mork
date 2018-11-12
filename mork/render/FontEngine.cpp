#include "mork/render/FontEngine.h"
#include "mork/core/Log.h"
#include "mork/render/Font.h"

#include <freetype/include/ft2build.h>
#include FT_FREETYPE_H

namespace mork {

    std::map<std::string, std::shared_ptr<Font>> FontEngine::fonts;
    
    std::string FontEngine::font_vs = 
R"(#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;
out vec2 TexCoords;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(pos.xy, 0.0, 1.0);
    TexCoords = tex;
})";  

	std::string FontEngine::font_fs = 
R"(#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    color = vec4(textColor, 1.0) * sampled;
})";   
    
    std::shared_ptr<Program> FontEngine::fontProgram = nullptr;
    
    void FontEngine::init() {

        fontProgram = std::make_shared<Program>(FontEngine::font_vs, FontEngine::font_fs);
	    
        FT_Library ft;
        if (FT_Init_FreeType(&ft)) {
            throw std::runtime_error("FREETYPE: Could not init FreeType Library");
        }
        
        // TODO: Config externally or pack fonts with source?
        std::map<std::string, std::string> fontsToLoad;
        fontsToLoad["Ubuntu"] = "../resources/fonts/Ubuntu-R.ttf";
        fontsToLoad["Liberation Mono"] = "../resources/fonts/LiberationMono-Regular.ttf";
        fontsToLoad["Liberation Sans"] = "../resources/fonts/LiberationSans-Regular.ttf";

        std::map<std::string, FT_Face> faces;
        
        for(auto& pair : fontsToLoad) {
            FT_Face face;
              
            if (FT_New_Face(ft, pair.second.c_str(), 0, &face)) {
                error_logger("Error loading font \"", pair.first, "\", path=", pair.second);
                throw std::runtime_error("FREETYPE: Failed to load font");
            }
            faces[pair.first] = face;
        }

       
                
        info_logger("Loading fonts:");

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

        for(auto& pair : faces) {
            FT_Set_Pixel_Sizes(pair.second, 0, 48); 

            auto font = std::make_shared<Font>();;

            FT_Face face = pair.second;
            // Load the standard characters
            for(unsigned char c = 0; c < 128; ++c) {
                auto glyph = std::make_shared<Glyph>();

				// Load character glyph 
                if (FT_Load_Char(pair.second, c, FT_LOAD_RENDER))
                {
                    warn_logger("ERROR::FREETYTPE: Failed to load Glyph, font: ", pair.first, ", character: [", c, "].. Font will be skipped");
                    continue;
                }                

                glyph->texture.loadTexture(
                        face->glyph->bitmap.width,
                        face->glyph->bitmap.rows,
                        1, // Only one channel, freetype is monochrome
                        face->glyph->bitmap.buffer);

                glyph->size = vec2i(face->glyph->bitmap.width, face->glyph->bitmap.rows);
                glyph->bearing = vec2i(face->glyph->bitmap_left, face->glyph->bitmap_top);
                glyph->advance = face->glyph->advance.x;

                font->characters.insert(std::make_pair(static_cast<char>(c), glyph));

                int x = 0;
            }
        
            fonts.insert(std::make_pair(pair.first, font)); 
             
        
        
        
        
        
            info_logger("  - ", pair.first);
            
            // Cleanup freetype faces
            FT_Done_Face(pair.second);
        }

        FT_Done_FreeType(ft);



    }
    Font& FontEngine::getFont(const std::string& name) {
        if(fonts.count(name)==0) {
            error_logger("Error looking up font ", name, ". Available fonts are:");
            for(auto& p : fonts)
                error_logger(" - ", p.first);   
            throw std::runtime_error("Font not found."); 
        }

        return *fonts[name];
    }


    Program& FontEngine::getProgram(){
        return *fontProgram;

    }

}
