#ifndef _MORK_GPURBUFFER_H_
#define _MORK_GPURBUFFER_H_

#include <vector>

#include "mork/render/Bindable.h"
#include "mork/glad/glad.h"
#include "mork/ui/GlfwWindow.h"


namespace mork {
    template <typename T, GLenum target, GLenum usage>
    class GPUBuffer : public Bindable {

    public:
        GPUBuffer() {
            glGenBuffers(1, &bufptr);
        }
        virtual ~GPUBuffer() {
            // TODO: Change method of establishing active context
            // (do not use GLFWWindow)
            if(GlfwWindow::isContextActive())
            {
                unbind();
                glDeleteBuffers(1, &bufptr);
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
    private:

        unsigned int bufptr;
    };


}


#endif
