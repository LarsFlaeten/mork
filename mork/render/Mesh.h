#ifndef _MORK_MESH_H_
#define _MORK_MESH_H_

#include "mork/math/box3.h"
#include "mork/render/VertexBuffer.h"
#include "mork/render/VertexArrayObject.h"

namespace mork {

    // Virtual interface for drawable meshes
    class MeshBase {
    public:
        virtual void draw() const = 0;

    };


    template<typename vertex>
    class Mesh : public MeshBase {
        public:
            Mesh(const Mesh&) = delete;
            Mesh& operator=(const Mesh&) = delete;
            Mesh(Mesh&&) = default;
            Mesh& operator=(Mesh&&) = default;

            Mesh(const std::vector<vertex>& vertices)
                : materialIndex(0) {
                setVertices(vertices);
            }

            Mesh(const std::vector<vertex>& vertices, const std::vector<unsigned int>& indices)
                : materialIndex(0) {
                setVerticesIndexed(vertices, indices);
            }
            
            Mesh(const std::vector<vertex>& vertices, unsigned int materialIndex)
                : materialIndex(materialIndex) {
                setVertices(vertices);
            }

            Mesh(const std::vector<vertex>& vertices, const std::vector<unsigned int> indices, unsigned int materialIndex) 
            : materialIndex(materialIndex)
            {
                setVerticesIndexed(vertices, indices);
            }


            void setVertices(const std::vector<vertex>& vertices) {
                indexed = false;
                numVertices = vertices.size();
                numIndices = 0;
                vao.bind();
                vb.bind();
                vb.setData(vertices);
                vb.setAttributes();
                vb.unbind();
                vao.unbind();
                bounds = calculateBounds(vertices);
            }

            void setVerticesIndexed(const std::vector<vertex>& vertices, const std::vector<unsigned int>& indices) {
                indexed = true;
                numVertices = vertices.size();
                numIndices = indices.size();
                vao.bind();
                vb.bind();
                vb.setData(vertices);
                vb.setAttributes();
                ib.bind();
                ib.setData(indices);
                vb.unbind();
                vao.unbind();
                bounds = calculateBounds(vertices);
            }

            int getNumVertices() const {
                return numVertices;
            }

            const VertexBuffer<vertex>& getVertexBuffer() const {
                return vb;
            }

            const IndexBuffer& getIndexBuffer() const {
                return ib;
            } 
            
            int getNumIndices() const {
                return numIndices;
            }
            
            bool isIndexed() const {
                return indexed;
            }

            unsigned int getMaterialIndex() const {
                return materialIndex;
            }

            void setMaterialIndex(unsigned int index) {
                materialIndex = index;
            }

            virtual void draw() const {
                vao.bind();
                
                if(!indexed) 
                    glDrawArrays(GL_TRIANGLES, 0, numVertices); 
                else {
                    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0); 
                }

                vao.unbind();

            }

            box3d getBounds() const {
                return bounds;
            }
    

        private:

            box3d calculateBounds(const std::vector<vertex> vertices) {
                box3d b = box3d::ZERO;
                
                // Set initial values
                if(vertices.size()>0) {
                    const auto& p = mork::vec3f(vertices[0].pos);

                    b.xmax = p.x;
                    b.xmin = p.x;
                    b.ymax = p.y;
                    b.ymin = p.y;
                    b.zmax = p.z;
                    b.zmin = p.z;
                }

                for(auto& v : vertices) {
                    auto p = mork::vec3f(v.pos);

                    if(p.x > b.xmax)
                        b.xmax = p.x;

                    if(p.x < b.xmin)
                        b.xmin = p.x;

                    if(p.y > b.ymax)
                        b.ymax = p.y;

                    if(p.y < b.ymin)
                        b.ymin = p.y;

                    if(p.z > b.zmax)
                        b.zmax = p.z;

                    if(p.z < b.zmin)
                        b.zmin = p.z;

                }
                return b;

            }

            bool indexed;
            int numVertices;
            int numIndices;

            // An index to a material resource pool stored elswhere
            unsigned int                materialIndex;
            
            mork::VertexBuffer<vertex>  vb;
            mork::IndexBuffer           ib;
            mork::VertexArrayObject     vao;      

            mork::box3d bounds;

    };

    template<typename T>
    class MeshHelper {};

    template <> class MeshHelper<vertex_pos2> {
        public:
            static Mesh<vertex_pos2> PLANE();
    };
 
    template <> class MeshHelper<vertex_pos3> {
        public:
            static Mesh<vertex_pos3> PLANE();
    };
 
    template <> class MeshHelper<vertex_pos4> {
        public:
            static Mesh<vertex_pos4> PLANE();
    };
 
    template <> class MeshHelper<vertex_pos_norm_uv> {
        public:
            static Mesh<vertex_pos_norm_uv> PLANE();
            static Mesh<vertex_pos_norm_uv> BOX();
            static Mesh<vertex_pos_norm_uv> SPHERE(
                    double rx, 
                    double ry, 
                    double rz, 
                    unsigned int stacks, 
                    unsigned int sectors);


    };
    
    template <> class MeshHelper<vertex_pos_norm_tang_bitang_uv> {
        public:
            static Mesh<vertex_pos_norm_tang_bitang_uv> PLANE();
            static Mesh<vertex_pos_norm_tang_bitang_uv> BOX();
            static Mesh<vertex_pos_norm_tang_bitang_uv> SPHERE(
                    double rx, 
                    double ry, 
                    double rz, 
                    unsigned int stacks, 
                    unsigned int sectors);


    };
}


#endif
