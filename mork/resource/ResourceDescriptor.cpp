#include "mork/resource/ResourceDescriptor.h"

namespace mork {

    ResourceDescriptor::ResourceDescriptor(const json& j)
        : descriptor(j)
    {

    }

    const json& ResourceDescriptor::getDescriptor() const {
        return descriptor;
    }

    json ResourceDescriptor::getDescriptor(){
        return descriptor;
    }



}
