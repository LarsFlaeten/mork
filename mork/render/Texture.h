#ifndef _MORK_TEXTURE_H_
#define _MORK_TEXTURE_H_

#include <string>

#include "mork/render/Bindable.h"
#include "mork/glad/glad.h"
#include "mork/core/Log.h"

namespace mork {

   
    class TextureBase : public Bindable {
    public:
        TextureBase();
        virtual ~TextureBase();
        TextureBase(TextureBase&& o);

      
        virtual void bind(int texUnit) const;
        virtual void unbind(int texUnit) const;


        virtual void loadTexture(const std::string& file, bool flip_vertical) = 0;
        
        virtual int getWidth() const = 0;
        virtual int getHeight() const = 0;
        virtual int getDepth() const = 0;
        virtual int getNumChannels() const = 0;

    protected:
        virtual void bind() const = 0;
        virtual void unbind() const = 0;
       
        struct TextureData {
            int width;
            int height;
            int numChannels;
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
                td.width = 0;
                td.height = 0;
                td.numChannels = 0;
            }

            Texture<2>(Texture<2> && other) {
                texture = other.texture;
                other.texture = 0;
                td = other.td;
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
             
            virtual void loadTexture(int width, int height, int numChannels, unsigned char* data) {
                TextureData td;
                td.width = width;
                td.height = height;
                td.numChannels = numChannels;
                td = loadTexture2D(texture, td, data, true);
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
            
            virtual int getNumChannels() const {
                return td.numChannels;
            }

        protected:
            virtual void bind() const {
                glBindTexture(GL_TEXTURE_2D, texture);
            }
    
            virtual void unbind() const {
                glBindTexture(GL_TEXTURE_2D, 0);
            }
 
        private:
            TextureData td;
    };

   

}

#endif

