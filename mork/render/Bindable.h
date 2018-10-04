#ifndef _MORK_BINDABLE_H_
#define _MORK_BINDABLE_H_

// Interface for bindable Opengl resources
// 

namespace mork {

class Bindable {
        virtual void bind() const = 0;
        virtual void unbind() const = 0;
};


}


#endif
