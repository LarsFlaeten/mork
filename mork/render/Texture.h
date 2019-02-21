#ifndef _MORK_TEXTURE_H_
#define _MORK_TEXTURE_H_

#include <string>
#include <vector>

#include "mork/render/Bindable.h"
#include "mork/glad/glad.h"
#include "mork/core/Log.h"

namespace mork {

   
    class TextureBase : public Bindable {
    public:
        TextureBase();
        virtual ~TextureBase();
        TextureBase(TextureBase&& o) noexcept;
        TextureBase& operator=(TextureBase&& o) noexcept;


      
        virtual void bind(int texUnit) const;
        virtual void unbind(int texUnit) const;


        virtual void loadTexture(const std::string& file, bool flip_vertical) = 0;
        
        virtual int getWidth() const = 0;
        virtual int getHeight() const = 0;
        virtual int getDepth() const = 0;
        virtual int getFormat() const = 0;
            
        virtual bool operator==(const TextureBase& other) const;
        virtual bool operator!=(const TextureBase& other) const;

        unsigned int getTextureId() const;

        virtual void bind() const = 0;
        virtual void unbind() const = 0;
 
    protected:
      
        struct TextureData {
            int width;
            int height;
            int format;
        };

        unsigned int texture;

        virtual TextureData loadTexture2D(unsigned int texture, const std::string& file, bool flip_vertical, bool generate_mip);

        virtual TextureData loadTexture2D(unsigned int texture, const TextureData& td, unsigned char* data, bool generate_mip);



    };

    template <int dim>
    class Texture : public TextureBase {

    };
 
    template<> class Texture<2> : public TextureBase
    {
        public:
            Texture<2>() : TextureBase(){
                debug_logger("Texture<2> CTOR");
                td.width = 0;
                td.height = 0;
                td.format = -1;
            }

            Texture<2>(Texture<2> && other) noexcept {
                texture = other.texture;
                other.texture = 0;
                td = other.td;
            }
            
            Texture<2>& operator=(Texture<2>&& o) noexcept {
                texture = o.texture; o.texture = 0;
                td = o.td;  
                return *this;

            }

            virtual void bind(int texUnit) const {
                TextureBase::bind(texUnit);
            }
    
            virtual void unbind(int texUnit) const {
                TextureBase::unbind(texUnit);
            }
   
            virtual void loadTexture(const std::string& file, bool flip_vertical) {
                td = loadTexture2D(texture, file, flip_vertical, true);
            }
             
            virtual void loadTexture(int width, int height, int internalformat, unsigned char* data, bool generateMip) {
                TextureData td;
                td.width = width;
                td.height = height;
                td.format = internalformat;
                td = loadTexture2D(texture, td, data, generateMip);
            }
            
            virtual int getWidth() const {
                return td.width;
            }
            
            virtual int getHeight() const{
                return td.height;
            }
            
            virtual int getDepth() const {
                return 1;
            }
            
            virtual int getFormat() const {
                return td.format;
            }

            virtual void bind() const {
                glBindTexture(GL_TEXTURE_2D, texture);
            }
    
            virtual void unbind() const {
                glBindTexture(GL_TEXTURE_2D, 0);
            }
 
        protected:
            TextureData td;
    };

    // The only specialization of a cubemap texture is the bind methods
    // and the loading of the texture
    class CubeMapTexture : public Texture<2> {
        public:        
        CubeMapTexture();
        CubeMapTexture(CubeMapTexture&& o) noexcept;
        CubeMapTexture& operator=(CubeMapTexture&& o) noexcept;

        virtual void bind() const;
        virtual void unbind() const;
        virtual void bind(int texUnit) const;
        virtual void unbind(int texUnit) const;
 
        virtual void loadTextures(const std::vector<std::string>& face_paths);
        
        private:
        
        // Hide these:
        virtual void loadTexture(const std::string& file, bool flip_vertical);
        virtual void loadTexture(int width, int height, int internalformat, unsigned char* data, bool generateMip);


 


    };
   

}

#endif

