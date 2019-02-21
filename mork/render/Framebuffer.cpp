#include "mork/render/Framebuffer.h"
#include "mork/ui/GlfwWindow.h"

#include <stdexcept>

namespace mork {

    Framebuffer Framebuffer::DEFAULT_FRAMEBUFFER = Framebuffer(true);

    const Framebuffer* Framebuffer::ACTIVE_FRAMEBUFFER = nullptr;

    Framebuffer::Framebuffer(bool main)
        : clearColor(vec4f(0.0f, 0.0f, 0.0f, 1.0f)),
        size(vec2i::ZERO) // Size is set later on default framebuffer
    {
        if(main) {
            fbo = 0;
            return;
        } else {
            // SHould really never be here
            throw std::runtime_error("This CTOR is for creating the default framebuffer only");

        }
    }

    Framebuffer::Framebuffer(int width, int height)
        : clearColor(vec4f(0.0f, 0.0f, 0.0f, 1.0f)),
            colorBuffer(std::make_unique<Texture<2> >()),
            depthStencilBuffer(std::make_unique<Texture<2> >()),
            size(vec2i(width, height))
    {
        
                
        glGenFramebuffers(1, &fbo);

        allocateBuffers();

        attachBuffers();
    }

    void Framebuffer::allocateBuffers() {        

        if(colorBuffer->getTextureId()>0) { 
            colorBuffer->bind();
            auto tex = colorBuffer->getTextureId();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
            colorBuffer->unbind();
        }

        // TODO: Do the same below as above
        if(depthStencilBuffer->getTextureId()>0) {
            depthStencilBuffer->bind();
            auto dsb = depthStencilBuffer->getTextureId();
            glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, size.x, size.y, 0, 
                          GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
                    );
            depthStencilBuffer->unbind();
        }


    }

    void Framebuffer::attachBuffers() {        
        bind();
        if(colorBuffer->getTextureId()>0) { 
            auto tex = colorBuffer->getTextureId();
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
        }

        if(depthStencilBuffer->getTextureId()>0) {
            auto dsb = depthStencilBuffer->getTextureId();
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, dsb, 0);
        }
        unbind();

    }


    Framebuffer::~Framebuffer() {
        // quick way out if fbo == 0;
        if(!fbo)
            return;

        // TODO: Change method of establishing active context
        // (do not use GLFWWindow)
        if(GlfwWindow::isContextActive())
        {
            if(fbo)
                glDeleteFramebuffers(1, &fbo);
        } 
 
    }

    void Framebuffer::bind() const {
        ACTIVE_FRAMEBUFFER = this;
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glViewport(0, 0, size.x, size.y);
    }

    void Framebuffer::unbind() const {

        // Release this target by binind default framebuffer
        Framebuffer::getDefault().bind();
    }

    void Framebuffer::clear() const {
        auto& activeFB = Framebuffer::getActive();

        if(ACTIVE_FRAMEBUFFER!= this)
            this->bind();
        
        auto& c = clearColor;
        glClearColor(c.x, c.y, c.z, c.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Rebind previous target
        if(&activeFB != this) {
            activeFB.bind();
        }
    }

    void Framebuffer::setSize(vec2i _size) {
        
        if(_size != size) {
            size = _size;

            // Resize textures:
            if(colorBuffer && depthStencilBuffer)
                allocateBuffers();
        }

        if(Framebuffer::ACTIVE_FRAMEBUFFER == this)
           glViewport(0, 0, size.x, size.y); 
    }

    vec2i Framebuffer::getSize() const {
        return size;
    }

    void Framebuffer::setClearColor(vec4f color) {
        clearColor = color;
    }

    vec4f Framebuffer::getClearColor() const {
        return clearColor;
    } 

    const Texture<2>& Framebuffer::getColorBuffer() const {
        return *colorBuffer;
    }

    Framebuffer& Framebuffer::getDefault() {
        return DEFAULT_FRAMEBUFFER;
    }

    const Framebuffer& Framebuffer::getActive() {
        return *ACTIVE_FRAMEBUFFER;
    }

    void Framebuffer::validateProgram(const Program& prog) {
        glValidateProgram(prog.getProgramID());
        GLint status = 0;
        glGetProgramiv(prog.getProgramID(), GL_VALIDATE_STATUS, &status);
        if(status != GL_TRUE) {
            GLint maxlength = 0;
            glGetProgramiv(prog.getProgramID(), GL_INFO_LOG_LENGTH, &maxlength);
            mork::error_logger("Program validation failed on program ID=", prog.getProgramID());
            if(maxlength > 0) {
                GLchar info[maxlength+1];
                glGetProgramInfoLog(prog.getProgramID(), maxlength+1, NULL, info);
                std::string loginfo(info);
                mork::error_logger(loginfo);
            }
            throw std::runtime_error("Program validation failed, see log for details");

        }
    }
 
}