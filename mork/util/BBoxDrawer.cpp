#include "mork/util/BBoxDrawer.h"

#include "mork/render/Material.h"


namespace mork {
    const char *vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "uniform mat4 projection;\n"
        "uniform mat4 view;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = projection * view * vec4(aPos, 1.0);\n"
        "}\0";
    const char *fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "\n"
        "uniform vec3 color;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(color, 1.0);\n"
        "}\n\0";

    std::unique_ptr<Program> BBoxDrawer::prog;
  
    bool BBoxDrawer::initialized = false; 
    
    std::unique_ptr<VertexArrayObject>   BBoxDrawer::vao;
    
    std::unique_ptr<DynamicVertexBuffer<vertex_pos> > BBoxDrawer::buf;
 
    void BBoxDrawer::drawBox(const box3d& box, const mat4d& projection, const mat4d& view) {

        if(!initialized) {
            vao = std::make_unique<VertexArrayObject>();
            buf = std::make_unique<DynamicVertexBuffer<vertex_pos> >(); 
            prog = std::make_unique<Program>(vertexShaderSource, fragmentShaderSource);
            vao->bind();
            buf->bind();
            
            buf->setAttributes();
            buf->setBufferSize(sizeof(vertex_pos)*24);
             
            buf->unbind();
            vao->unbind();    

            initialized = true;
        }

        prog->use();
        prog->getUniform("color").set(vec3f(1.0f, 1.0f, 1.0f));
        prog->getUniform("projection").set(projection.cast<float>());
        prog->getUniform("view").set(view.cast<float>());

        float xp = box.xmax;
        float xm = box.xmin;
        float yp = box.ymax;
        float ym = box.ymin;
        float zp = box.zmax;
        float zm = box.zmin;

        std::vector<vertex_pos> verts = {
            // 12 lines in total
            
            vec3f(xp, yp, zp),
            vec3f(xp, ym, zp),
            vec3f(xp, ym, zp),
            vec3f(xp, ym, zm),
            vec3f(xp, ym, zm),
            vec3f(xp, yp, zm),
            vec3f(xp, yp, zm),
            vec3f(xp, yp, zp),
            
            vec3f(xp, yp, zp),
            vec3f(xm, yp, zp),
            //vec3f(xm, yp, zp),
            //vec3f(xm, yp, zm),
            vec3f(xm, yp, zm),
            vec3f(xp, yp, zm),
            //vec3f(xp, yp, zm),
            //vec3f(xp, yp, zp),
 
            vec3f(xm, yp, zp),
            vec3f(xm, ym, zp),
            vec3f(xm, ym, zp),
            vec3f(xm, ym, zm),
            vec3f(xm, ym, zm),
            vec3f(xm, yp, zm),
            vec3f(xm, yp, zm),
            vec3f(xm, yp, zp),
            
            vec3f(xp, ym, zp),
            vec3f(xm, ym, zp),
            //vec3f(xm, ym, zp),
            //vec3f(xm, ym, zm),
            vec3f(xm, ym, zm),
            vec3f(xp, ym, zm),
            //vec3f(xp, ym, zm),
            //vec3f(xp, ym, zp),
        };
        
        buf->setData(verts);

        // Prepare drawing:
        vao->bind();
        glDrawArrays(GL_LINES, 0, 24);        


    }



}
