#ifndef _MORK_RESOURCE_RESOURCETEMPLATE_H_
#define _MORK_RESOURCE_RESOURCETEMPLATE_H_

#include "mork/resource/ResourceManager.h"
#include "mork/resource/ResourceDescriptor.h"
#include <string>
#include <nlohmann/json.hpp>
#include <json-schema.hpp>

using json = nlohmann::json;
using nlohmann::json_schema::json_validator;



namespace mork {


	template<typename T>
	class ResourceTemplate {
	   public:
		/**
		 * Creates a new %resource of class T.
		 *
		 * @param schema the json schema for this resource
		 */
		ResourceTemplate(const json& schema)
		{
			validator.set_root_schema(schema);
		}

		/**
		 * Releases the created resource for use by the program.
		 * After this method is called, the resource temaplate no longer
		 * holds the resource, and it must be recreated again if a new object is needed.
		 */
		virtual T releaseResource() = 0;

	protected:
		json schema;
		json_validator validator;
	};




}

#endif
