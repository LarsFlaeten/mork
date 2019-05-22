#include "Mesh.h"

#include "mork/resource/ResourceFactory.h"
#include "mork/util/Util.h"
#include <mork/util/MeshUtil.h>
#include <mork/core/Log.h>


#include <cmath>

namespace mork {

using VN = vertex_pos_norm_uv;
using VTBN = vertex_pos_norm_tang_bitang_uv;

Mesh<vertex_pos2> MeshHelper<vertex_pos2>::PLANE() {
    const std::vector<unsigned int> indices = {  
        1, 0, 3,  // first Triangle
        2, 1, 3   // second Triangle
    };


    const std::vector<mork::vertex_pos2> vertices = {
        // positions                    
        {mork::vec2f(1.0f,  1.0f)},   // top right
        {mork::vec2f(1.0f,  -1.0f)},   // bottom right
        {mork::vec2f(-1.0f,  -1.0f)},   // bottom left
        {mork::vec2f(-1.0f,  1.0f)}    // top left 
    }; 

    return Mesh<vertex_pos2>(vertices, indices);
    
}

Mesh<vertex_pos3> MeshHelper<vertex_pos3>::PLANE() {
    const std::vector<unsigned int> indices = {  
        1, 0, 3,  // first Triangle
        2, 1, 3   // second Triangle
    };


    const std::vector<mork::vertex_pos3> vertices = {
        // positions                    
        {mork::vec3f(1.0f,  1.0f, 0.0f)},   // top right
        {mork::vec3f(1.0f,  -1.0f, 0.0f)},   // bottom right
        {mork::vec3f(-1.0f,  -1.0f, 0.0f)},   // bottom left
        {mork::vec3f(-1.0f,  1.0f, 0.0f)}    // top left 
    }; 

    return Mesh<vertex_pos3>(vertices, indices);
    
}

Mesh<vertex_pos4> MeshHelper<vertex_pos4>::PLANE() {
    const std::vector<unsigned int> indices = {  
        1, 0, 3,  // first Triangle
        2, 1, 3   // second Triangle
    };


    const std::vector<mork::vertex_pos4> vertices = {
        // positions                    
        {mork::vec4f(1.0f,  1.0f, 0.0f, 1.0f)},   // top right
        {mork::vec4f(1.0f,  -1.0f, 0.0f, 1.0f)},   // bottom right
        {mork::vec4f(-1.0f,  -1.0f, 0.0f, 1.0f)},   // bottom left
        {mork::vec4f(-1.0f,  1.0f, 0.0f, 1.0f)}    // top left 
    }; 

    return Mesh<vertex_pos4>(vertices, indices);
    
}



Mesh<vertex_pos_norm_uv> MeshHelper<vertex_pos_norm_uv>::PLANE() {
    const std::vector<unsigned int> indices = {  
        1, 0, 3,  // first Triangle
        2, 1, 3   // second Triangle
    };


    mork::vec3f normal = mork::vec3f(0.0f,0.0f,1.0f);
    const std::vector<mork::vertex_pos_norm_uv> vertices = {
        // positions                    // texture coords
        {mork::vec3f(1.0f,  1.0f, 0.0f),  normal,  mork::vec2f(1.0f, 1.0f)},   // top right
        {mork::vec3f(1.0f,  -1.0f, 0.0f), normal,   mork::vec2f(1.0f, 0.0f)},   // bottom right
        {mork::vec3f(-1.0f,  -1.0f, 0.0f), normal,   mork::vec2f(0.0f, 0.0f)},   // bottom left
        {mork::vec3f(-1.0f,  1.0f, 0.0f), normal,  mork::vec2f(0.0f, 1.0f)}    // top left 
    }; 

    return Mesh<vertex_pos_norm_uv>(vertices, indices);
    
}

Mesh<VTBN> MeshHelper<VTBN>::PLANE() {

    auto mesh = MeshHelper<VN>::PLANE();
    return MeshUtil::calculateTBNMesh(mesh);
}
    

Mesh<vertex_pos_norm_uv> MeshHelper<vertex_pos_norm_uv>::BOX() {
    std::vector<unsigned int> indices = {
        0, 1, 2,
        1, 0, 3,
        6, 7, 8,
        9, 10, 11,
        12, 13, 14,
        15, 16, 17,
        18, 19, 20,
        21, 22, 23,
        24, 25, 26,
        27, 28, 29,
        30, 31, 32,
        33, 34, 35

    };

    std::vector<mork::vertex_pos_norm_uv> vertices = {
        {mork::vec3f(-1.0f, -1.0f, -1.0f), mork::vec3f(0,0,-1), mork::vec2f(0.0f, 0.0f)},
        {mork::vec3f( 1.0f,  1.0f, -1.0f), mork::vec3f(0,0,-1), mork::vec2f(1.0f, 1.0f)},
        {mork::vec3f( 1.0f, -1.0f, -1.0f), mork::vec3f(0,0,-1), mork::vec2f(1.0f, 0.0f)},
        {mork::vec3f(-1.0f,  1.0f, -1.0f), mork::vec3f(0,0,-1), mork::vec2f(0.0f, 1.0f)},
        {mork::vec3f( 1.0f,  1.0f, -1.0f), mork::vec3f(0,0,-1), mork::vec2f(1.0f, 1.0f)},
        {mork::vec3f(-1.0f, -1.0f, -1.0f), mork::vec3f(0,0,-1), mork::vec2f(0.0f, 0.0f)},

        {mork::vec3f(-1.0f, -1.0f,  1.0f), mork::vec3f(0,0,1),  mork::vec2f(0.0f, 0.0f)},
        {mork::vec3f( 1.0f, -1.0f,  1.0f), mork::vec3f(0,0,1),  mork::vec2f(1.0f, 0.0f)},
        {mork::vec3f( 1.0f,  1.0f,  1.0f), mork::vec3f(0,0,1),  mork::vec2f(1.0f, 1.0f)},
        {mork::vec3f( 1.0f,  1.0f,  1.0f), mork::vec3f(0,0,1),  mork::vec2f(1.0f, 1.0f)},
        {mork::vec3f(-1.0f,  1.0f,  1.0f), mork::vec3f(0,0,1),  mork::vec2f(0.0f, 1.0f)},
        {mork::vec3f(-1.0f, -1.0f,  1.0f), mork::vec3f(0,0,1),  mork::vec2f(0.0f, 0.0f)},

        {mork::vec3f(-1.0f,  1.0f,  1.0f), mork::vec3f(-1,0,0),  mork::vec2f(1.0f, 0.0f)},
        {mork::vec3f(-1.0f,  1.0f, -1.0f), mork::vec3f(-1,0,0),  mork::vec2f(1.0f, 1.0f)},
        {mork::vec3f(-1.0f, -1.0f, -1.0f), mork::vec3f(-1,0,0),  mork::vec2f(0.0f, 1.0f)},
        {mork::vec3f(-1.0f, -1.0f, -1.0f), mork::vec3f(-1,0,0),  mork::vec2f(0.0f, 1.0f)},
        {mork::vec3f(-1.0f, -1.0f,  1.0f), mork::vec3f(-1,0,0),  mork::vec2f(0.0f, 0.0f)},
        {mork::vec3f(-1.0f,  1.0f,  1.0f), mork::vec3f(-1,0,0),  mork::vec2f(1.0f, 0.0f)},

        {mork::vec3f(1.0f,  1.0f,  1.0f), mork::vec3f(1,0,0),  mork::vec2f(1.0f, 0.0f)},
        {mork::vec3f(  1.0f, -1.0f, -1.0f), mork::vec3f(1,0,0),  mork::vec2f(0.0f, 1.0f)},
        {mork::vec3f( 1.0f,  1.0f, -1.0f), mork::vec3f(1,0,0),  mork::vec2f(1.0f, 1.0f)},
        {mork::vec3f( 1.0f, -1.0f, -1.0f), mork::vec3f(1,0,0),  mork::vec2f(0.0f, 1.0f)},
        {mork::vec3f( 1.0f,  1.0f,  1.0f), mork::vec3f(1,0,0),  mork::vec2f(1.0f, 0.0f)},
        {mork::vec3f( 1.0f, -1.0f,  1.0f), mork::vec3f(1,0,0),  mork::vec2f(0.0f, 0.0f)},
 
        {mork::vec3f(-1.0f, -1.0f, -1.0f), mork::vec3f(0,-1,0),  mork::vec2f(0.0f, 1.0f)},
        {mork::vec3f( 1.0f, -1.0f, -1.0f), mork::vec3f(0,-1,0),  mork::vec2f(1.0f, 1.0f)},
        {mork::vec3f( 1.0f, -1.0f,  1.0f), mork::vec3f(0,-1,0),  mork::vec2f(1.0f, 0.0f)},
        {mork::vec3f( 1.0f, -1.0f,  1.0f), mork::vec3f(0,-1,0),  mork::vec2f(1.0f, 0.0f)},
        {mork::vec3f(-1.0f, -1.0f,  1.0f), mork::vec3f(0,-1,0),  mork::vec2f(0.0f, 0.0f)},
        {mork::vec3f(-1.0f, -1.0f, -1.0f), mork::vec3f(0,-1,0),  mork::vec2f(0.0f, 1.0f)},

        {mork::vec3f(-1.0f,  1.0f, -1.0f), mork::vec3f(0,1,0),  mork::vec2f(0.0f, 1.0f)},
        {mork::vec3f( 1.0f,  1.0f,  1.0f), mork::vec3f(0,1,0),  mork::vec2f(1.0f, 0.0f)},
        {mork::vec3f( 1.0f,  1.0f, -1.0f), mork::vec3f(0,1,0),  mork::vec2f(1.0f, 1.0f)},
        {mork::vec3f( 1.0f,  1.0f,  1.0f), mork::vec3f(0,1,0),  mork::vec2f(1.0f, 0.0f)},
        {mork::vec3f(-1.0f,  1.0f, -1.0f), mork::vec3f(0,1,0),  mork::vec2f(0.0f, 1.0f)},
        {mork::vec3f(-1.0f,  1.0f,  1.0f), mork::vec3f(0,1,0),  mork::vec2f(0.0f, 0.0f)}
    };       

    return Mesh<vertex_pos_norm_uv>(vertices, indices);
    
}

Mesh<VTBN> MeshHelper<VTBN>::BOX() {

    auto mesh = MeshHelper<VN>::BOX();
    return MeshUtil::calculateTBNMesh(mesh);
}
 
Mesh<vertex_pos_norm_uv> MeshHelper<vertex_pos_norm_uv>::SPHERE(double rx, double ry, double rz, unsigned int stacks, unsigned int sectors) {

    // http://www.songho.ca/opengl/gl_sphere.html
    double x, y, z, xy;
    double nx, ny, nz;
    double u, v;

    double d_sector = 2.0 * M_PI / sectors;
    double d_stack = M_PI / stacks;
    
    double sector_angle, stack_angle;    

    bool sphere = true;
    if (rx != ry || ry != rz || rx != rz) 
        sphere = false;

    std::vector<vertex_pos_norm_uv> vertices;

    for(unsigned int i = 0; i <= stacks; ++i) {
        stack_angle = -0.5*M_PI + i * d_stack; // From PI/2 to -PI/2
        z = rz * ::sin(stack_angle);
        double cos_stack_angle = ::cos(stack_angle);

        for(unsigned int j = 0; j <= sectors; ++j) {
            sector_angle = -M_PI + j*d_sector; // From -PI to  PI
            
            // Position
            x = rx * cos_stack_angle * ::cos(sector_angle);
            y = ry * cos_stack_angle * ::sin(sector_angle);
            vec3d pos(x, y, z);

            // If radii are different along the basis vectors, 
            // Normals have to be calculated throug the indices,
            // since pos.normalize() only works with a perfect sphere
            vec3 norm = vec3d::ZERO;
            if (sphere) 
                vec3d norm = pos.normalize();
            
            // Textur coords:
            u = (double)j / sectors;
            v = (double)i / stacks;
            vec2d uv(u, v);

            vertex_pos_norm_uv vert(pos.cast<float>(), norm.cast<float>(), uv.cast<float>());
            vertices.push_back(vert);
        }

    }

    std::vector<unsigned int> indices;
    int k1, k2;
    for(unsigned int i = 0; i < stacks; ++i) {
        k1 = i * (sectors + 1);     // beginning of current stack
        k2 = k1 + sectors + 1;      // beginning of next stack
        for(unsigned int j = 0; j < sectors; ++j, ++k1, ++k2) {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if(i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k1 + 1);
                indices.push_back(k2);
 
                vec3f e1 = vertices[k1 + 1].pos - vertices[k1].pos;
                vec3f e2 = vertices[k2].pos - vertices[k1].pos;
                vec3f norm = e1.crossProduct(e2);
                vertices[k1].norm += norm;
                vertices[k2].norm += norm;
                vertices[k1 + 1].norm += norm;



            }

            // k1+1 => k2 => k2+1
            if(i != (stacks-1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2 + 1);
                indices.push_back(k2);
                
                vec3f e1 = vertices[k2 + 1].pos - vertices[k1 + 1].pos;
                vec3f e2 = vertices[k2].pos - vertices[k1 + 1].pos;
               
                vec3f norm = e1.crossProduct(e2);
                vertices[k1 + 1].norm += norm;
                vertices[k2].norm += norm;
                vertices[k2 + 1].norm += norm;

           }
        }
    }

    for(auto& v : vertices) {
        v.norm = v.norm.normalize();
    }

    return Mesh<vertex_pos_norm_uv>(vertices, indices); 
}
 
Mesh<VTBN> MeshHelper<VTBN>::SPHERE(double rx, double ry, double rz, unsigned int stacks, unsigned int sectors) {
    auto _sphere = MeshHelper<VN>::SPHERE(rx, ry, rz, stacks, sectors);
    auto sphereTBN = MeshUtil::calculateTBNMesh(_sphere); 
    return sphereTBN;
}
 
    inline json meshSchema = R"(
    {
        "$schema": "http://json-schema.org/draft-07/schema#",
        "title": "Mesh schema",
        "type": "object",
        "description": "A mesh object",
        "oneOf": [
			{
                "properties" : {
            	    "sphere": { "type": "object" },
                    "materialIndex": { "type": "integer" }
                },
                "required": ["sphere", "materialIndex"],
                "additionalProperties": false
            },
			{
                "properties" : {
            	    "box": { "type": "object" },
                    "materialIndex": { "type": "integer" }
                },
                "required": ["box", "materialIndex"],
                "additionalProperties": false
            },
			{
                "properties" : {
            	    "plane": { "type": "object" },
                    "materialIndex": { "type": "integer" }
                },
                "required": ["plane", "materialIndex"],
                "additionalProperties": false
            }
		]
    }
    )"_json;


    class MeshResource: public ResourceTemplate<Mesh<VTBN> >
    {
		public:
		    MeshResource(ResourceManager& manager, Resource& r) :
				ResourceTemplate<Mesh<VTBN> >(meshSchema)
			{
				info_logger("Resource - Mesh");
            	const json& js = r.getDescriptor();
                validator.validate(js);
                
                materialIndex = -1;
                if(js.count("materialIndex")) {               
				    materialIndex = js["materialIndex"].get<int>();
                    if(materialIndex < 0)
                        warn_logger("Warning, materialIndex < 0 specificed, ignoring");    
                }
                resource = js;
		    }

            Mesh<VTBN> releaseResource() {
				if(resource.count("sphere")) {
                    json sphere = resource["sphere"];
                    vec3d r = string2vec3d(sphere["radii"]);
                    
                    int stacks = 40;
                    int slices = 80;
                    if(sphere.count("slices"))
                        slices = sphere["slices"].get<int>();
                    if(sphere.count("stacks"))
                        slices = sphere["stacks"].get<int>();
                    if(slices < 0 || stacks < 0)
                        throw std::invalid_argument("Sphere: slices/stacks cannot be negative");

                    auto m = MeshHelper<VTBN>::SPHERE(r.x, r.y, r.z, stacks, slices);
                    if(materialIndex > 0)
                        m.setMaterialIndex(materialIndex);
                    return std::move(m);
				}
				else
                    throw std::runtime_error("Unknown resource,\n " + resource.get<std::string>());


            }
		private:
            json resource;
			int materialIndex;		

    };

    inline std::string mesh = "mesh";

    static ResourceFactory<Mesh<VTBN> >::Type<mesh, MeshResource> MeshType;




}
