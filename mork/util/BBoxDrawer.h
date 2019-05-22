#ifndef _mork_bboxdrawer_h_
#define _mork_bboxdrawer_h_

#include "mork/math/box3.h"
#include "mork/render/Program.h"
#include "mork/render/VertexBuffer.h"
#include "mork/render/VertexArrayObject.h"

namespace mork {

    class BBoxDrawer {

        public:
            static void drawBox(const box3d& box, const mat4d& projection, const mat4d& view);

        private:
            static std::unique_ptr<VertexArrayObject>   vao;
            static std::unique_ptr<DynamicVertexBuffer<vertex_pos3> > buf;
            static std::unique_ptr<Program> prog;
            static bool initialized;
    };
}



#endif
