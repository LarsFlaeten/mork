#ifndef __MORK_RESOURCEFACTORY_H_
#define __MORK_RESOURCEFACTORY_H_

#include "mork/resource/ResourceManager.h"
#include "mork/resource/ResourceDescriptor.h"
#include "mork/resource/ResourceTemplate.h"
#include "mork/core/Log.h"

#include <nlohmann/json.hpp>
#include <json-schema.hpp>

#include <string>
#include <map>

using json = nlohmann::json;
using nlohmann::json_schema::json_validator;

namespace mork {

template <typename T>
class ResourceFactory {
    public:
            // Singleton pattern the new way...
            // https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
            static ResourceFactory& getInstance() {
                static ResourceFactory resf;
                return resf;
            }

            ResourceFactory(const ResourceFactory&) = delete;
            void operator=(const ResourceFactory&) = delete;

            typedef T (*createFunc) (
                    ResourceManager& manager,
                    Resource& r);


            /**
             * Registers a new %resource type with this factory.
             *
             * @param type a %resource type, as it appears in the tag of a
             *      ResourceDescriptor (e.g. texture1D, texture2D, shader, program, etc).
             * @param f a function that can create %resources of this type.
             */
            void addType(const std::string &type, createFunc f) {
                types[type] = f;
            }

            template<const std::string& typeName, typename RT>
            class Type {
                public:
                static T ctor(ResourceManager& manager,
                                Resource& r)
                {
                    // Create the resource via resourceTemplate
                    auto rt = RT(manager, r);
                    return rt.releaseResource();
                }

                Type()
                {
                    ResourceFactory::getInstance().addType(typeName, ctor);
                }

            };

            T create(ResourceManager& manager, Resource& r) {
                
                typename
                std::map<std::string, createFunc>::iterator i = types.find(r.getType());

                if (i != types.end()) {
                    return i->second(manager, r);
                } else {
                    throw std::runtime_error("Unknown type " + r.getType());
                }
            }

            T create(ResourceManager& manager, const std::string& name) {
                
                auto& resource = manager.getResource(name);
                return std::move(this->create(manager, resource));
            }
       private:
            ResourceFactory() {}


            /**
             * The registered creation functions. Maps %resource types (such as shader,
             * program, mesh, etc) to %resource creation functions.
             */
            std::map<std::string, createFunc> types;
};

    
}


#endif
