#ifndef _MORK_MESH_H_
#define _MORK_MESH_H_

#include "mork/math/box3.h"
#include "mork/render/VertexBuffer.h"
#include "mork/render/VertexArrayObject.h"

namespace mork {

    template<typename vertex>
    class Mesh {
        public:

            Mesh(const std::vector<vertex>& vertices)
                : materialIndex(0) {
                setVertices(vertices);
            }

            Mesh(const std::vector<vertex>& vertices, const std::vector<unsigned int> indices)
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

            void setVerticesIndexed(const std::vector<vertex>& vertices, const std::vector<unsigned int> indices) {
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

            void draw() const {
                vao.bind();
                
                if(!indexed) 
                    glDrawArrays(GL_TRIANGLES, 0, numVertices); 
                else {
                    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0); 
                }

            }

            box3d getBounds() const {
                return bounds;
            }
    

        private:

            box3d calculateBounds(const std::vector<vertex> vertices) {
                box3d b = box3d::ZERO;
                
                // Set initial values
                if(vertices.size()>0) {
                    const auto& p = vertices[0].pos;

                    b.xmax = p.x;
                    b.xmin = p.x;
                    b.ymax = p.y;
                    b.ymin = p.y;
                    b.zmax = p.z;
                    b.zmin = p.z;
                }

                for(auto& v : vertices) {
                    if(v.pos.x > b.xmax)
                        b.xmax = v.pos.x;

                    if(v.pos.x < b.xmin)
                        b.xmin = v.pos.x;

                    if(v.pos.y > b.ymax)
                        b.ymax = v.pos.y;

                    if(v.pos.y < b.ymin)
                        b.ymin = v.pos.y;

                    if(v.pos.z > b.zmax)
                        b.zmax = v.pos.z;

                    if(v.pos.z < b.zmin)
                        b.zmin = v.pos.z;

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
}


#endif
