#include "mork/render/Texture.h"
#include "mork/ui/GlfwWindow.h"

#include "mork/core/Log.h"
#include "mork/core/stb_image.h"

#include <stdexcept>

namespace mork {

    TextureBase::TextureBase() : texture(0) {
        glGenTextures(1, &texture);
    }


    TextureBase::~TextureBase() {
        // TODO: Change method of establishing active context
        // (do not use GLFWWindow)
        if(GlfwWindow::isContextActive())
        {
            if(texture)
                glDeleteTextures(1, &texture);
        } 
    }

    void TextureBase::bind(int texUnit) {
        if(texUnit < 0)
            throw std::runtime_error("texUnit < 0 not allowed");
        glActiveTexture(GL_TEXTURE0 + texUnit);
        bind();
    }

    void TextureBase::unbind(int texUnit) {
        if(texUnit < 0)
            throw std::runtime_error("texUnit < 0 not allowed");
        glActiveTexture(GL_TEXTURE0 + texUnit);
        unbind();
    }   

    TextureBase::TextureData TextureBase::loadTexture2D(unsigned int texture, const std::string& file, bool flip_vertical = false, bool generate_mip = true)
    {
        TextureBase::TextureData td;
        
        stbi_set_flip_vertically_on_load(flip_vertical);

        unsigned char* data = stbi_load(file.c_str(), &td.width, &td.height, &td.numChannels, 0);
       
        if(!data) {
           mork::error_logger("File \"", file, "\" could not be loaded.");           
           throw std::runtime_error("Error loading image, see logs");
        }

        // Set default texture wrapping/filtering option
        // TODO: Make options for ajusting wrapping and min/mag filtering
        bind();
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if(generate_mip)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        else
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 
        
        auto format = td.numChannels==4 ? GL_RGBA : ( td.numChannels==3 ? GL_RGB : 0);

        glTexImage2D(GL_TEXTURE_2D, 0, format, td.width, td.height, 0, format, GL_UNSIGNED_BYTE, data);
        if(generate_mip)
            glGenerateMipmap(GL_TEXTURE_2D);
        
        unbind();

       

        stbi_image_free(data);

        return td;
    }


 
}
