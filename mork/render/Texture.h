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

        virtual void bind() const = 0;
        virtual void unbind() const = 0;
        
        virtual void bind(int texUnit);
        virtual void unbind(int texUnit);


        virtual void loadTexture(const std::string& file, bool flip_vertical) = 0;
        
        virtual int getWidth() const = 0;
        virtual int getHeight() const = 0;
        virtual int getDepth() const = 0;
        virtual int getNumChannels() const = 0;

    protected:
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
            virtual void bind() const {
                glBindTexture(GL_TEXTURE_2D, texture);
            }
    
            virtual void unbind() const {
                glBindTexture(GL_TEXTURE_2D, 0);
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

       private:
            TextureData td;
    };

   

}

#endif

