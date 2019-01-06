#ifndef _MORK_MODELIMPORTER_H_
#define _MORK_MODELIMPORTER_H_

#include <string>

#include "mork/render/Model.h"


namespace mork {

    class ModelImporter {
        public:
            static Model   loadModel(const std::string& path, const std::string& file, const std::string& nodeName);
 

    };



}

#endif
