#ifndef _MORK_UTIL_MESHUTIL_H_
#define _MORK_UTIL_MESHUTIL_H_

#include <mork/render/Mesh.h>

namespace mork {

    class MeshUtil {
        public:
        // Converts a mesh with normals and uvs to also include tangents and bitangents:
        static Mesh<vertex_pos_norm_tang_bitang_uv> calculateTBNMesh(const BasicMesh& in);
        
        // Performs TBN calculations based on input vertices and indices
        static std::vector<vertex_pos_norm_tang_bitang_uv> calculateTangentSpace(
            const std::vector<vertex_pos_norm_uv>& vertices, 
            const std::vector<unsigned int> indices);
 
    };


}

#endif
