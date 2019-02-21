#include "mork/render/Texture.h"
#include "mork/ui/GlfwWindow.h"

#include "mork/core/Log.h"
#include "mork/core/stb_image.h"

#include <stdexcept>

namespace mork {

    TextureBase::TextureBase() : texture(0) {
        glGenTextures(1, &texture);
    }

    TextureBase::TextureBase(TextureBase&& other) noexcept : texture(other.texture) {
        other.texture = 0;
    }

    TextureBase& TextureBase::operator=(TextureBase&& other) noexcept {
        if(other.texture != texture) {
            texture = other.texture;
            other.texture = 0;
        }
        return *this;
    }


    TextureBase::~TextureBase() {
        // quick way out if texture == 0;
        if(!texture)
            return;

        // TODO: Change method of establishing active context
        // (do not use GLFWWindow)
        if(GlfwWindow::isContextActive())
        {
            if(texture)
                glDeleteTextures(1, &texture);
        } 
    }

    void TextureBase::bind(int texUnit) const {
        if(texUnit < 0)
            throw std::runtime_error("texUnit < 0 not allowed");
        glActiveTexture(GL_TEXTURE0 + texUnit);
        bind();
    }

    void TextureBase::unbind(int texUnit) const {
        if(texUnit < 0)
            throw std::runtime_error("texUnit < 0 not allowed");
        glActiveTexture(GL_TEXTURE0 + texUnit);
        unbind();
    }   
        
    bool TextureBase::operator==(const TextureBase& other) const {
        return texture == other.texture;
    }
        
    bool TextureBase::operator!=(const TextureBase& other) const {
        return texture != other.texture;
    }

    unsigned int TextureBase::getTextureId() const {
        return texture;
    }


    TextureBase::TextureData TextureBase::loadTexture2D(unsigned int texture, const std::string& file, bool flip_vertical = false, bool generate_mip = true)
    {
        TextureBase::TextureData td;
        
        stbi_set_flip_vertically_on_load(flip_vertical);



        int numChannels;
        unsigned char* data = stbi_load(file.c_str(), &td.width, &td.height, &numChannels, 0);
       
        if(!data) {
           mork::error_logger("File \"", file, "\" could not be loaded.");           
           throw std::runtime_error("Error loading image, see logs");
        }

        // Guess internal format from stbis numChannels:
        td.format = (numChannels == 4) ? GL_RGBA8 : ( numChannels == 3 ? GL_RGB8 : ( numChannels == 2 ? GL_RG8 : ( numChannels == 1 ? GL_R8 : -1 ) ) );

        td =  loadTexture2D(texture, td, data, generate_mip);
        
        stbi_image_free(data);

        return td;

   }

    TextureBase::TextureData TextureBase::loadTexture2D(unsigned int texture, const TextureData& td, unsigned char* data, bool generate_mip = true) {


        // Set default texture wrapping/filtering option
        // TODO: Make options for ajusting wrapping and min/mag filtering
        bind(0);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if(generate_mip)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        else
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 
        
        auto format = td.format== GL_RGBA8 ? GL_RGBA :
            ( td.format==GL_RGB8 ? GL_RGB :
             ( td.format==GL_RG8 ? GL_RG : 
               (td.format==GL_R8 ? GL_RED :
                (td.format==GL_DEPTH24_STENCIL8 ? GL_DEPTH_STENCIL
                  : -1 // defaul
                 )
                )
               )
            );
    
        // SET type to unsigned byte unsless it is a depth/stencil buffer:
        auto type = td.format==GL_DEPTH24_STENCIL8 ? GL_UNSIGNED_INT_24_8 : GL_UNSIGNED_BYTE;

        glTexImage2D(GL_TEXTURE_2D, 0, td.format, td.width, td.height, 0, format, type, data);
        if(generate_mip)
            glGenerateMipmap(GL_TEXTURE_2D);
        
        unbind(7);

       

        return td;
    }
    
    CubeMapTexture::CubeMapTexture() :  Texture<2>()  {}

    CubeMapTexture::CubeMapTexture(CubeMapTexture&& o) noexcept {
                texture = o.texture;
                td = o.td;
                o.texture = 0;
    }
            
    CubeMapTexture& CubeMapTexture::operator=(CubeMapTexture&& o) noexcept {
                texture = o.texture; 
                o.texture = 0;
                td = o.td;  
                return *this;

    }


    void CubeMapTexture::bind() const {
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    }
    
    void CubeMapTexture::unbind() const {
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
    
    void CubeMapTexture::bind(int texUnit) const {
        TextureBase::bind(texUnit);
    }
    
    void CubeMapTexture::unbind(int texUnit) const {
        TextureBase::unbind(texUnit);
    }


    void CubeMapTexture::loadTextures(const std::vector<std::string>& face_paths) {
		bind(0);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < face_paths.size(); i++)
		{
			unsigned char *data = stbi_load(face_paths[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
							 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
				);
				stbi_image_free(data);
			}
			else
			{
                mork::error_logger("Cubemap texture failed to load at path: ", face_paths[i]);
                throw std::runtime_error("Failed to load cubemap texture");
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        unbind(1);
    }


    void CubeMapTexture::loadTexture(const std::string& file, bool flip_vertical) {}

    void CubeMapTexture::loadTexture(int width, int height, int internalformat, unsigned char* data, bool generateMip) {}


}
