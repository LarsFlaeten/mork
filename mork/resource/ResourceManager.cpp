#include "mork/resource/ResourceManager.h"

#include "mork/core/Log.h"
#include "mork/util/File.h"

#include <fstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

using namespace nlohmann;

namespace mork {

    Resource::Resource(ResourceManager& resManager, const std::string& type, const json& desc, const std::string& filePath)
        :  manager(&resManager), type(type), descriptor(desc), filePath(filePath)
    {
        createDateTime = std::chrono::system_clock::now();
    }

    const std::string& Resource::getType() const {
        return type;
    }
    
    const json& Resource::getDescriptor() const {
        return descriptor;
    }

    const std::string& Resource::getFilePath() const {
        return filePath;
    }
    
    const std::chrono::system_clock::time_point Resource::getCreateDateTime() const {
        return createDateTime;
    }

    Resource&  Resource::addChildResource(Resource&& r) {
        childResources.push_back(std::move(r));
        return childResources[childResources.size() - 1];
    }
        
    std::vector<Resource>& Resource::getChilds() {
        return childResources;
    }

    const std::vector<Resource>& Resource::getChilds() const {
        return childResources;
    }


    bool Resource::needUpdate() const {

        // Get the last write time of the file path:
        if(!filePath.empty()) {
            auto modTime = getLastModifiedTime(filePath);
            auto dur = createDateTime - modTime;
            if (dur < std::chrono::milliseconds(0))
                return true;
        }

        // Return true if one of the children needs update
        for( const auto& child : childResources) {
            if(child.needUpdate())
                return true;
        }

        return false;

    }
    ResourceManager::ResourceManager() {
    
    }
        
    ResourceManager::ResourceManager(const std::string& file) {
        loadResource(file);
    }

    void ResourceManager::loadResource(const std::string& file) {
        std::ifstream is(file);
        json j;
        is >> j;
        for(auto el = j.begin(); el != j.end(); ++el) {
            auto& name = el.value()["name"];
            addResource(name, el.key(), el.value(), file);
        }
    }

    void ResourceManager::loadResource(const std::string& file, const std::string& resourceName) {
        std::ifstream is(file);
        json j;
        is >> j;
        for(auto el = j.begin(); el != j.end(); ++el) {
            auto name = el.value()["name"].get<std::string>();
            if(name.compare(resourceName) == 0) {
                addResource(name, el.key(), el.value(), file);
                return;    
            }
        }
    }


    const Resource& ResourceManager::addResource(const std::string& name, const std::string& type, const json& desc, const std::string& filePath) {
        if(resources.count(name)) {
            throw std::runtime_error("Resource with name " + name + " allready present in resources.");
        }
        auto it = resources.emplace(std::make_pair(name, Resource(*this, type, desc, filePath)));
      
        const auto& r = (*(it.first)).second; 
        return r;
        // DONETODO: Optimize, as this is uneccesary lookup. The resource can be found in it above 
        //return resources.at(name);
    }

    const Resource& ResourceManager::getResource(const std::string& name)  const {
        auto it = resources.find(name);
        if( it != resources.end())
            return resources.find(name)->second;
        else {
            throw std::runtime_error("Resource " + name + " not found");
        }
    }

    Resource& ResourceManager::getResource(const std::string& name){
        auto it = resources.find(name);
        if( it != resources.end())
            return resources.find(name)->second;
        else {
            throw std::runtime_error("Resource " + name + " not found");
        }
    }

 
    const std::unordered_map<std::string, Resource >& ResourceManager::Resources() const {
        return resources;
    }

    void ResourceManager::removeResource(const std::string& name) {
        resources.erase(name);
    }

    std::string ResourceManager::dumpKeys() const {
        std::stringstream s;
        for(const auto& kv : resources)
            s << kv.first << "\n";
        return s.str();
    }

    std::ostream& operator << (std::ostream& os, const Resource& r)
    {
        os << "Type:        [" << r.getType() << "]\n";
        os << "Loaded from: <" << r.getFilePath() << ">\n";
        os << "CDT:         " << r.getCreateDateTime() << "\n";
        os << "Childs:      "  << r.getChilds().size() << "\n";
        os << "Descriptor:\n";
        os << r.getDescriptor() << "\n";
        return os;
    }

    std::ostream& operator << (std::ostream& os, const ResourceManager& r)
    {
        os << "****** Resource list ******\n";
        for( const auto& [key,value] : r.Resources() )
        {
            os << "Resource name: \"" << key << "\"\n";
            os << value;

        }
        
        
        return os;
    }







}
