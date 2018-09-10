#ifndef _MORK_BINDABLE_H_
#define _MORK_BINDABLE_H_

// Interface for bindable Opengl resources
// 

namespace mork {

class Bindable {
        virtual void bind() = 0;
        virtual void unbind() = 0;
};


}


#endif
