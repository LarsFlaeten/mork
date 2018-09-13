#ifndef _MORK_VERTEXBUFFER_H_
#define _MORK_VERTEXBUFFER_H_
#include "mork/render/GPUBuffer.h"

namespace mork {
//template<typename vertex> 
//using VertexBuffer = GPUBuffer<vertex, GL_ARRAY_BUFFER, GL_STATIC_DRAW>;

template<typename vertex>
class VertexBuffer : public GPUBuffer<vertex, GL_ARRAY_BUFFER, GL_STATIC_DRAW> {
    public:
        virtual void setAttributes() {
            vertex::setAttributes();
        }

};


    /*
public:
    VertexBuffer() {
        glGenBuffers(1, &vbo);
    }

    virtual ~VertexBuffer() {
        // TODO: Change method of establishing active context
        // (do not use GLFWWindow)
        if(GlfwWindow::isContextActive())
        {
            unbind();
            glDeleteBuffers(1, &vbo);
        }   

    }

    virtual void setData(std::vector<vertex> vertices) {
        this->bind();
        this->vertices = vertices; 
        glBufferData(GL_ARRAY_BUFFER, this->getSize(), &vertices[0] , GL_STATIC_DRAW);
    
        vertex::setAttributes();
    }



    virtual void bind() {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    }

    virtual void unbind() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

private:
    GLsizeiptr getSize() {
        return this->vertices.size()*sizeof(vertex);
    }

    std::vector<vertex> vertices;
    unsigned int vbo;

}; 
*/
}

#endif
