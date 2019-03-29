#include "MeshUtil.h"

namespace mork {
    
    Mesh<vertex_pos_norm_tang_bitang_uv> MeshUtil::calculateTBNMesh(const Mesh<vertex_pos_norm_uv>& in) {
        
        // Create a vertex vector with dummy objects and correct size
        auto n_v = in.getNumVertices();
        auto s_v = sizeof(vertex_pos_norm_uv);
        auto n_i = in.getNumIndices();
        auto s_i = sizeof(unsigned int);
        
        // Map the incoming buffers using a scoped buffer views:
        auto& vb = in.getVertexBuffer();
        auto& ib = in.getIndexBuffer();
        auto vb_bw = ConstBufferView<VertexBuffer<vertex_pos_norm_uv> >(vb);
        auto ib_bw = ConstBufferView<IndexBuffer>(ib);

        // Copy the content to the vertex  and index vectors:
        vertex_pos_norm_uv a_verts[n_v];
        unsigned int a_indices[n_i];

        std::memcpy(a_verts, vb_bw.get(), n_v*s_v);
        std::vector<vertex_pos_norm_uv> vertices(a_verts, a_verts + n_v);

        std::memcpy(a_indices, ib_bw.get(), n_i*s_i);
        std::vector<unsigned int> indices(a_indices, a_indices + n_i);

        auto tbn_vertices = MeshUtil::calculateTangentSpace(vertices, indices);
        auto mesh = Mesh<vertex_pos_norm_tang_bitang_uv>(std::move(tbn_vertices), std::move(indices)); 
        return mesh;

    }



    
    // Calculate bitangent according to https://learnopengl.com/Advanced-Lighting/Normal-Mapping
    std::vector<vertex_pos_norm_tang_bitang_uv> MeshUtil::calculateTangentSpace(
            const std::vector<vertex_pos_norm_uv>& vertices, 
            const std::vector<unsigned int>& indices) {
        
        std::vector<vertex_pos_norm_tang_bitang_uv> t_vertices;
        t_vertices.resize(vertices.size());

        for(int i = 0; i < indices.size(); i+=3)
        {
            auto v1 = vertices[indices[i]];
            auto v2 = vertices[indices[i+1]];
            auto v3 = vertices[indices[i+2]];

            vec3f edge1 = v2.pos - v1.pos;
            vec3f edge2 = v3.pos - v1.pos;

            vec2f deltaUV1 = v2.uv - v1.uv;
            vec2f deltaUV2 = v3.uv - v1.uv;

            float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

            vec3f tangent(            
                (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
                (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
                (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z) );
            
            vec3f bitangent(
                f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x),
                f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y),
                f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z) );

            auto tv1 = t_vertices[indices[i]];
            auto tv2 = t_vertices[indices[i+1]];
            auto tv3 = t_vertices[indices[i+2]];

            tv1.pos = v1.pos;
            tv1.uv = v1.uv;
            tv1.norm += v1.norm;
            tv1.tang += tangent;
            tv1.bitang += bitangent;
            
            tv2.pos = v2.pos;
            tv2.uv = v2.uv;
            tv2.norm += v2.norm;
            tv2.tang += tangent;
            tv2.bitang += bitangent;
            
            tv3.pos = v3.pos;
            tv3.uv = v3.uv;
            tv3.norm += v3.norm;
            tv3.tang += tangent;
            tv3.bitang += bitangent;
 
            t_vertices[indices[i]] = tv1;
            t_vertices[indices[i+1]] = tv2;
            t_vertices[indices[i+2]] = tv3;



        }
        
        // perform normalization/smoothing and reorthogonalization
        for(auto& v : t_vertices) {
            auto T = v.tang.normalize();
            auto B = v.bitang.normalize();
            auto N = v.norm.normalize();

            T = (T - T.dotproduct(N)*N).normalize();
            B = N.crossProduct(T);

            v.tang = T;
            v.bitang = B;
            v.norm = N;
        }

        return std::move(t_vertices);

    }



}
