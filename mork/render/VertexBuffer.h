#ifndef _MORK_VERTEXBUFFER_H_
#define _MORK_VERTEXBUFFER_H_
#include "mork/math/vec2.h"
#include "mork/math/vec3.h"
#include "mork/math/vec4.h"
#include "mork/render/GPUBuffer.h"

namespace mork {

template<typename vertex>
class VertexBuffer : public GPUBuffer<vertex, GL_ARRAY_BUFFER, GL_STATIC_DRAW> {
    public:
        virtual void setAttributes() {
            vertex::setAttributes();
        }

};

template<typename vertex>
class DynamicVertexBuffer : public GPUBuffer<vertex, GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW> {
    public:
        virtual void setAttributes() {
            vertex::setAttributes();
        }

};


struct vertex_pos {
    mork::vec3f   pos;

    vertex_pos(const mork::vec3f& p) {pos = p;};

    inline static void setAttributes() {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }
};

struct vertex_pos_uv {
    mork::vec3f   pos;
    mork::vec2f   uv;
    vertex_pos_uv(const mork::vec3f& p, mork::vec2f u) {pos = p; uv = u;};

    inline static void setAttributes() {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
    }
};


struct vertex_pos_col {
    mork::vec3f   pos;
    mork::vec4f   col;

    vertex_pos_col(const mork::vec3f& p, const mork::vec4f& c) : pos(p), col(c) {};

    inline static void setAttributes() {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
    }
};

struct vertex_pos_norm_uv {
    mork::vec3f     pos;
    mork::vec3f     norm;
    mork::vec2f     uv;

    vertex_pos_norm_uv(const mork::vec3f& p, const mork::vec3f& n, const mork::vec2f u) : pos(p), norm(n), uv(u) {};

    inline static void setAttributes() {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
        
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
        glEnableVertexAttribArray(2);
    }
};



struct vertex_pos_col_uv {
    mork::vec3f     pos;
    mork::vec4f     col;
    mork::vec2f     uv;

    vertex_pos_col_uv(const mork::vec3f& p, const mork::vec4f& c, const mork::vec2f u) : pos(p), col(c), uv(u) {};

    inline static void setAttributes() {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
        
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7*sizeof(float)));
        glEnableVertexAttribArray(2);
    }
};




}

#endif
