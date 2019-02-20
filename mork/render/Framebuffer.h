#ifndef _MORK_FRAMEBUFFER_H_
#define _MORK_FRAMEBUFFER_H_

#include <mork/render/Bindable.h>
#include <mork/render/Texture.h>
#include <mork/render/Program.h>
#include <mork/math/vec2.h>
#include <mork/math/vec4.h>

namespace mork {

    class Framebuffer : public Bindable {
        public:
            // Creates a new offscreen framebuffer
            // For now we assume a standard framebuffer width a color atachment and
            //  a depth/stencil buffer.
            //  TODO: Enable more advanced randertarget framebuffers
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

            // Sets the color to be used when clearing the buffer
            void setClearColor(vec4f color);

            // Returns the current color used for clearing the buffer
            vec4f getClearColor() const; 

            // Returns the underlying color buffer
            const Texture<2>& getColorBuffer() const;

            // Returns the default (on-screen) framebuffer
            static Framebuffer& getDefault();
            
            // Returns the active Framebuffer
            static const Framebuffer& getActive();

            // Validates the program given, and logs the error + throws exception if validation
            // fails
            static void validateProgram(const Program& prog);

        private:
            void allocateBuffers();
            void attachBuffers();


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
