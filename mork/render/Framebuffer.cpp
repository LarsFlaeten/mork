#include "mork/render/Framebuffer.h"
#include "mork/ui/GlfwWindow.h"

#include <stdexcept>

namespace mork {

    Framebuffer Framebuffer::DEFAULT_FRAMEBUFFER = Framebuffer(true);

    const Framebuffer* Framebuffer::ACTIVE_FRAMEBUFFER = nullptr;

	/*
	<p>a function to test whether the RGB format is a supported renderbuffer color
	format (the OpenGL 3.3 Core Profile specification requires support for the RGBA
	formats, but not for the RGB ones):
	*/

	bool Framebuffer::isFramebufferRgbFormatSupported(bool half_precision) {
	  GLuint test_fbo = 0;
	  glGenFramebuffers(1, &test_fbo);
	  glBindFramebuffer(GL_FRAMEBUFFER, test_fbo);
	  GLuint test_texture = 0;
	  glGenTextures(1, &test_texture);
	  glBindTexture(GL_TEXTURE_2D, test_texture);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	  glTexImage2D(GL_TEXTURE_2D, 0, half_precision ? GL_RGB16F : GL_RGB32F,
				   1, 1, 0, GL_RGB, GL_FLOAT, NULL);
	  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
							 GL_TEXTURE_2D, test_texture, 0);
	  bool rgb_format_supported =
		  glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
	  glDeleteTextures(1, &test_texture);
	  glDeleteFramebuffers(1, &test_fbo);
	  return rgb_format_supported;
	}

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
           size(vec2i(width, height))
    {
        glCreateFramebuffers(1, &fbo);
    }
/*
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
        if(depthStencilBuffer && depthStencilBuffer->getTextureId()>0) {
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

        if(depthStencilBuffer && depthStencilBuffer->getTextureId()>0) {
            auto dsb = depthStencilBuffer->getTextureId();
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, dsb, 0);
        }
        unbind();

    }
*/
    void Framebuffer::attachColorBuffer(mork::TextureBase& t) {
        glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0,  t.getTextureId(), 0);
        glNamedFramebufferDrawBuffer(fbo, GL_COLOR_ATTACHMENT0);
    }



    void Framebuffer::attachColorBuffers(std::vector<std::reference_wrapper<mork::TextureBase>> t) {
        auto s = t.size();
        GLuint kDrawBuffers[s];
        for(unsigned int i = 0; i < s; ++i)
            kDrawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
        
        if(t.size()>max_color_attachments || t.size()< 1)
        {
            error_logger("Size of coloBuffer vecotr must be 1-", max_color_attachments, " in current implementation, given vector had ", t.size(), " elements.");
            throw std::runtime_error(error_logger.last());
        }

        unsigned int i = 0;
        for(auto& r : t) {
            glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0 + (i++), r.get().getTextureId(), 0);
        }

        glNamedFramebufferDrawBuffers(fbo, t.size(), kDrawBuffers);

    }

    void Framebuffer::clearAttachments() {
        for(unsigned int i = 0; i < max_color_attachments; ++i)
            glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0 + i, 0, 0);
 

    }

    void Framebuffer::attachDeptStencilhBuffer(mork::Texture<2>& dsb) {
        glNamedFramebufferTexture(fbo, GL_DEPTH_STENCIL_ATTACHMENT, dsb.getTextureId(), 0);
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
            //if(colorBuffer)
            //    allocateBuffers();
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

    Framebuffer& Framebuffer::getDefault() {
        return DEFAULT_FRAMEBUFFER;
    }

    const Framebuffer& Framebuffer::getActive() {
        return *ACTIVE_FRAMEBUFFER;
    }

    void Framebuffer::validateProgram(const Program& prog) {
        glValidateProgram(prog.getProgramId());
        GLint status = 0;
        glGetProgramiv(prog.getProgramId(), GL_VALIDATE_STATUS, &status);
        if(status != GL_TRUE) {
            GLint maxlength = 0;
            glGetProgramiv(prog.getProgramId(), GL_INFO_LOG_LENGTH, &maxlength);
            mork::error_logger("Program validation failed on program ID=", prog.getProgramId());
            if(maxlength > 0) {
                GLchar info[maxlength+1];
                glGetProgramInfoLog(prog.getProgramId(), maxlength+1, NULL, info);
                std::string loginfo(info);
                mork::error_logger(loginfo);
            }
            throw std::runtime_error("Program validation failed, see log for details");

        }
    }
 
}
