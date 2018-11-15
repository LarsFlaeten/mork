#include "VertexArrayObject.h"
#include "mork/glad/glad.h"
#include "mork/ui/GlfwWindow.h"
namespace mork {

VertexArrayObject::VertexArrayObject()
    : VAO(0) {
    glGenVertexArrays(1, &VAO);
}

VertexArrayObject::VertexArrayObject(VertexArrayObject&& o)
{
    // OUr VAO is not intialized at this time..
    VAO = o.VAO;
    o.VAO = 0;
}

VertexArrayObject& VertexArrayObject::operator=(VertexArrayObject&& o)
{

    // OUr VAO is intialized at this time..
    if(GlfwWindow::isContextActive())
    {
        if(VAO)
        {
            unbind();
            glDeleteVertexArrays(1, &VAO);
        }
    }
    
    VAO = o.VAO;
    o.VAO = 0;
}



VertexArrayObject::~VertexArrayObject() {
    // Guard against calling glDelete etc after 
    // OPenGL context is destrpyed (caousing annoying segfaults at shutdown)
    if(GlfwWindow::isContextActive())
    {
        if(VAO)
        {
            unbind();
            glDeleteVertexArrays(1, &VAO);
        }
    }
}

void VertexArrayObject::bind() const {
    glBindVertexArray(VAO);
    
}

void VertexArrayObject::unbind() const {
    glBindVertexArray(0);
}

}
