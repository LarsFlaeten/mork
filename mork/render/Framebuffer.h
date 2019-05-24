#ifndef _MORK_FRAMEBUFFER_H_
#define _MORK_FRAMEBUFFER_H_

#include <vector>
#include <functional>

#include <mork/render/Bindable.h>
#include <mork/render/Texture.h>
#include <mork/render/Program.h>
#include <mork/math/vec2.h>
#include <mork/math/vec4.h>

namespace mork {

    class Framebuffer : public Bindable {
		public:
			static bool isFramebufferRgbFormatSupported(bool half_precision);
	

        public:
            // Creates a new offscreen framebuffer
            Framebuffer(int width, int height);

            virtual ~Framebuffer();

            // Binds this framebuffer as the current render target
            virtual void bind() const;

            // Unbind this framebuffer
            // This essentially binds the default framebuffer, and is equivalent to calling
            // Framebuffer::getDefault().bind()
            virtual void unbind() const;

            // Clear this framebuffer
            void clear() const;

            // Sets the size of this framebuffer, and calls glViewport
            void setSize(vec2i size);

            // Returns the size of this framebuffer:
            vec2i getSize() const;

            // Attach a single colorbuffer to slot 0
            void attachColorBuffer(mork::TextureBase& t);

            // Attach a vector of colorbuffer texture references to slot 0, 1, ... n
            void attachColorBuffers(std::vector<std::reference_wrapper<mork::TextureBase>> t);

            // Removes tha current color buffer attachments
            void clearAttachments();

            // Attach a depth/stencil buffer
            void attachDeptStencilhBuffer(mork::Texture<2>& depthStencil);


            // Sets the color to be used when clearing the buffer
            void setClearColor(vec4f color);

            // Returns the current color used for clearing the buffer
            vec4f getClearColor() const; 

            // Returns the default (on-screen) framebuffer
            static Framebuffer& getDefault();
            
            // Returns the active Framebuffer
            static const Framebuffer& getActive();

            // Validates the program given, and logs the error + throws exception if validation
            // fails
            static void validateProgram(const Program& prog);

        private:
//            void allocateBuffers();
//            void attachBuffers();
            static constexpr int max_color_attachments = 4;

            // Creates a new framebuffer
            // main = true means the default on-screen framebuffer
            Framebuffer(bool main);

            std::unique_ptr<Texture<2> > colorBuffer;
            std::unique_ptr<Texture<2> > depthStencilBuffer;

            unsigned int fbo;

            vec2i       size;

            vec4f       clearColor;

            static Framebuffer  DEFAULT_FRAMEBUFFER;

            static const Framebuffer* ACTIVE_FRAMEBUFFER;
    };


}


#endif
