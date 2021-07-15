#ifndef _MORK_GPURBUFFER_H_
#define _MORK_GPURBUFFER_H_

#include <vector>

#include "mork/render/Bindable.h"
#include "mork/glad/glad.h"
#include "mork/ui/GlfwWindow.h"
#include "mork/core/Log.h"

namespace mork {
    enum BufferAccess {
        ReadOnly,
        WriteOnly,
        ReadWrite

    };

    // Scoped buffer view
    template<typename T>
    class BufferView {
        public:
        BufferView(const BufferView&) = delete;
        BufferView& operator=(const BufferView&) = delete;

        BufferView(T& buf, BufferAccess access) : t(buf) {
            ptr = buf.mapBuffer(access);
        }

        ~BufferView() {
            t.unmapBuffer();
        }

        void* get() {return ptr;}

        private:
            const T& t;
            void* ptr;
    };    
     
    // Scoped constant buffer view
    template<typename T>
    class ConstBufferView {
        public:
        ConstBufferView(const ConstBufferView&) = delete;
        ConstBufferView& operator=(const ConstBufferView&) = delete;


        ConstBufferView(const T& buf) : t(buf), ptr(buf.mapBuffer(BufferAccess::ReadOnly)) {}

        ~ConstBufferView() {
            t.unmapBuffer();
        }

        const void * get() {return ptr;}

        private:
            const T& t;
            const void* ptr;
    };    
    
    template <typename T, GLenum target, GLenum usage>
    class GPUBuffer : public Bindable {

    public:
        GPUBuffer() {
            glGenBuffers(1, &bufptr);
            assert(glGetError() == 0);
        }

        GPUBuffer(GPUBuffer&& o) noexcept {
            bufptr = o.bufptr;
            o.bufptr = 0;

        }

        GPUBuffer& operator=(GPUBuffer&& o) noexcept {
            if(GlfwWindow::isContextActive() && bufptr!=o.bufptr)
            {
                if(bufptr) {
                    unbind();
                    glDeleteBuffers(1, &bufptr);
                }
            }   
            
            bufptr = o.bufptr;
            o.bufptr = 0;
            
            return *this;

        }
        virtual ~GPUBuffer() {
            // TODO: Change method of establishing active context
            // (do not use GLFWWindow)
            if(GlfwWindow::isContextActive())
            {
                if(bufptr) {
                    unbind();
                    glDeleteBuffers(1, &bufptr);
                }
            }   
 
        }

        virtual void bind() const {
            glBindBuffer(target, bufptr);
        }
        virtual void unbind() const {
            glBindBuffer(target, 0);
 
        }

        virtual void setData(std::vector<T> data) {
            glNamedBufferData(bufptr, data.size()*sizeof(T), &data[0], usage);
        }
        
        // Sets an empty buffer with the given size
        // MOstly used with dynamic buffers at initialization
        virtual void setBufferSize(size_t size) {
            glNamedBufferData(bufptr, size, NULL, usage);
        }



        // Maps this buffer to a reachable adress space
        // As noted on the documentation, accessing a buffer in a way incompatible
        // with the buffer usage may be very slow
        T* mapBuffer(BufferAccess access) {
            switch(access) {
                case(ReadOnly):
                   return reinterpret_cast<T*>(glMapNamedBuffer(bufptr, GL_READ_ONLY));
                case(WriteOnly):
                   return reinterpret_cast<T*>(glMapNamedBuffer(bufptr, GL_WRITE_ONLY));
                case(ReadWrite):
                   return reinterpret_cast<T*>(glMapNamedBuffer(bufptr, GL_READ_WRITE));
                default:
                   return nullptr;
            }

        }

        // Maps this buffer to a reachable adress space
        // As noted on the documentation, accessing a buffer in a way incompatible
        // with the buffer usage may be very slow
        const T* mapBuffer(BufferAccess access) const{
            switch(access) {
                case(ReadOnly):
                    return reinterpret_cast<T*>(glMapNamedBuffer(bufptr, GL_READ_ONLY));
                case(WriteOnly):
                    mork::warn_logger("Tried to access const buffer with write access..");   
                    return nullptr;
                case(ReadWrite):
                    mork::warn_logger("Tried to access const buffer with write access..");   
                    return nullptr;
                default:
                    return nullptr;
            }

        }


        void unmapBuffer() const {
            glUnmapNamedBuffer(bufptr);
        }
    private:

        unsigned int bufptr;
    };


}


#endif
