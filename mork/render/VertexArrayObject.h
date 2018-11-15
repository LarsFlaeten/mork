#ifndef _MORK_VERTEXARRAYOBJECT_H_
#define _MORK_VERTEXARRAYOBJECT_H_

#include "Bindable.h"

namespace mork {

// A movable, but non-copyable OpenGl resource
class VertexArrayObject : public Bindable {
public:
    VertexArrayObject();
    virtual ~VertexArrayObject();
    
    VertexArrayObject(VertexArrayObject&& o);
    VertexArrayObject& operator=(VertexArrayObject&& o);

    virtual void bind() const;
    virtual void unbind() const;

private:
    unsigned int VAO;
};


}


#endif
