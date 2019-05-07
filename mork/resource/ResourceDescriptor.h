#ifndef _MORK_RESOURCEDESCRIPTOR_H_
#define _MORK_RESOURCEDESCRIPTOR_H_

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace mork {

    class ResourceDescriptor {
        public:
            ResourceDescriptor(const json& j);

            const json& getDescriptor() const;
            json getDescriptor();
       private:
            json descriptor;
    };



}


#endif

