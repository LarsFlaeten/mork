#ifndef __MORK_RESOURCEMANAGER_H_
#define __MORK_RESOURCEMANAGER_H_

#include "mork/resource/ResourceDescriptor.h"
#include "mork/util/Time.h"

#include <string>
#include <unordered_map>
#include <chrono>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace mork {

    class ResourceManager;

    class Resource {
        public:
            Resource(ResourceManager& manager, const std::string& type, const json& desc, const std::string& filePath);
        
            const json& getDescriptor() const;
            const std::string& getType() const;

            const std::string& getFilePath() const;
            const std::chrono::system_clock::time_point getCreateDateTime() const;

            Resource&  addChildResource(Resource&& r);
            std::vector<Resource>& getChilds();
            const std::vector<Resource>& getChilds() const;

            bool    needUpdate() const;

        private:
            ResourceManager*    manager;
            std::string         type;
            json                descriptor;
            std::string         filePath;
            std::chrono::system_clock::time_point createDateTime;

            std::vector<Resource> childResources;
    };


    class ResourceManager {
        public:
            ResourceManager();

            ResourceManager(const std::string& file);

            void loadResource(const std::string& file);

            void loadResource(const std::string& file, const std::string& resourceName);


            const Resource& addResource(const std::string& name, const std::string& type, const json& desc, const std::string& filePath);

            const Resource& getResource(const std::string& name)  const;

            Resource& getResource(const std::string& name);

            const std::unordered_map<std::string, Resource >& Resources() const;

            void removeResource(const std::string& name);
            std::string dumpKeys() const;

        private:
            std::unordered_map<std::string, Resource > resources;

    };

    std::ostream& operator << (std::ostream& os, const Resource& r);

    std::ostream& operator << (std::ostream& os, const ResourceManager& r);
  
}


#endif
