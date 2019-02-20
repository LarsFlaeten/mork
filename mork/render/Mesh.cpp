#include "Mesh.h"

namespace mork {

Mesh<vertex_pos_norm_uv> MeshHelper<vertex_pos_norm_uv>::PLANE() {
    const std::vector<unsigned int> indices = {  
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
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

Mesh<vertex_pos_norm_uv> MeshHelper<vertex_pos_norm_uv>::BOX() {
    std::vector<mork::vertex_pos_norm_uv> vertices = {
        {mork::vec3f(-1.0f, -1.0f, -1.0f), mork::vec3f(0,0,-1), mork::vec2f(0.0f, 0.0f)},
        {mork::vec3f( 1.0f, -1.0f, -1.0f), mork::vec3f(0,0,-1), mork::vec2f(1.0f, 0.0f)},
        {mork::vec3f( 1.0f,  1.0f, -1.0f), mork::vec3f(0,0,-1), mork::vec2f(1.0f, 1.0f)},
        {mork::vec3f( 1.0f,  1.0f, -1.0f), mork::vec3f(0,0,-1), mork::vec2f(1.0f, 1.0f)},
        {mork::vec3f(-1.0f,  1.0f, -1.0f), mork::vec3f(0,0,-1), mork::vec2f(0.0f, 1.0f)},
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
        {mork::vec3f( 1.0f,  1.0f, -1.0f), mork::vec3f(1,0,0),  mork::vec2f(1.0f, 1.0f)},
        {mork::vec3f(  1.0f, -1.0f, -1.0f), mork::vec3f(1,0,0),  mork::vec2f(0.0f, 1.0f)},
        {mork::vec3f( 1.0f, -1.0f, -1.0f), mork::vec3f(1,0,0),  mork::vec2f(0.0f, 1.0f)},
        {mork::vec3f( 1.0f, -1.0f,  1.0f), mork::vec3f(1,0,0),  mork::vec2f(0.0f, 0.0f)},
        {mork::vec3f( 1.0f,  1.0f,  1.0f), mork::vec3f(1,0,0),  mork::vec2f(1.0f, 0.0f)},

        {mork::vec3f(-1.0f, -1.0f, -1.0f), mork::vec3f(0,-1,0),  mork::vec2f(0.0f, 1.0f)},
        {mork::vec3f( 1.0f, -1.0f, -1.0f), mork::vec3f(0,-1,0),  mork::vec2f(1.0f, 1.0f)},
        {mork::vec3f( 1.0f, -1.0f,  1.0f), mork::vec3f(0,-1,0),  mork::vec2f(1.0f, 0.0f)},
        {mork::vec3f( 1.0f, -1.0f,  1.0f), mork::vec3f(0,-1,0),  mork::vec2f(1.0f, 0.0f)},
        {mork::vec3f(-1.0f, -1.0f,  1.0f), mork::vec3f(0,-1,0),  mork::vec2f(0.0f, 0.0f)},
        {mork::vec3f(-1.0f, -1.0f, -1.0f), mork::vec3f(0,-1,0),  mork::vec2f(0.0f, 1.0f)},

        {mork::vec3f(-1.0f,  1.0f, -1.0f), mork::vec3f(0,1,0),  mork::vec2f(0.0f, 1.0f)},
        {mork::vec3f( 1.0f,  1.0f, -1.0f), mork::vec3f(0,1,0),  mork::vec2f(1.0f, 1.0f)},
        {mork::vec3f( 1.0f,  1.0f,  1.0f), mork::vec3f(0,1,0),  mork::vec2f(1.0f, 0.0f)},
        {mork::vec3f( 1.0f,  1.0f,  1.0f), mork::vec3f(0,1,0),  mork::vec2f(1.0f, 0.0f)},
        {mork::vec3f(-1.0f,  1.0f,  1.0f), mork::vec3f(0,1,0),  mork::vec2f(0.0f, 0.0f)},
        {mork::vec3f(-1.0f,  1.0f, -1.0f), mork::vec3f(0,1,0),  mork::vec2f(0.0f, 1.0f)}
    };       

    return Mesh<vertex_pos_norm_uv>(vertices);
    
}






}
