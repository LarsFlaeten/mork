#ifndef _MORK_VERTEXARRAYOBJECT_H_
#define _MORK_VERTEXARRAYOBJECT_H_

#include "Bindable.h"

namespace mork {

class VertexArrayObject : public Bindable {
public:
    VertexArrayObject();
    virtual ~VertexArrayObject();

    virtual void bind();
    virtual void unbind();

private:
    unsigned int VAO;
};


}


#endif
