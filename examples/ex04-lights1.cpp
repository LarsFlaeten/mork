#include <iostream>
#include <stdexcept>
#include <vector>

#include <cxxopts.hpp>

#include "mork/ui/GlfwWindow.h"
#include "mork/core/Log.h"
#include "mork/render/Program.h"
#include "mork/render/VertexArrayObject.h"
#include "mork/math/vec3.h"
#include "mork/math/vec4.h"
#include "mork/render/VertexBuffer.h"
#include "mork/render/GPUBuffer.h"
#include "mork/render/StaticUniform.h"
#include "mork/render/Texture.h"

#include "mork/core/stb_image.h"

using namespace std;

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aUV;\n"
    "out vec2 texCoord;\n"
    "uniform mat4 transform;\n" 
    "void main()\n"
    "{\n"
    "   gl_Position = transform * vec4(aPos, 1.0);\n"
    "   texCoord = aUV;\n"
    "}\0";

const char *lampFragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 texCoord;\n"
    "\n"
    "uniform vec3 lightColor;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(lightColor, 1.0);\n"
    "}\n\0";


const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 texCoord;\n"
    "\n"
    "uniform sampler2D texture1;\n"
    "uniform sampler2D texture2;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   //FragColor = texture(texture1, texCoord) + 0.0*texture(texture2, texCoord);\n"
    "   FragColor = vec4(vec3(mix(texture(texture1, texCoord), texture(texture2, texCoord), 0.2)), 1.0);\n"
    "}\n\0";



class App : public mork::GlfwWindow {
public:
    App()
            : mork::GlfwWindow(mork::Window::Parameters().size(800,600)),
            prog(std::string(vertexShaderSource), std::string(fragmentShaderSource))
    {

        std::vector<mork::vertex_pos_uv> verts = {
            {mork::vec3f(-1.0f, -1.0f, -1.0f),  mork::vec2f(0.0f, 0.0f)},
            {mork::vec3f( 1.0f, -1.0f, -1.0f),  mork::vec2f(1.0f, 0.0f)},
            {mork::vec3f( 1.0f,  1.0f, -1.0f),  mork::vec2f(1.0f, 1.0f)},
            {mork::vec3f( 1.0f,  1.0f, -1.0f),  mork::vec2f(1.0f, 1.0f)},
            {mork::vec3f(-1.0f,  1.0f, -1.0f),  mork::vec2f(0.0f, 1.0f)},
            {mork::vec3f(-1.0f, -1.0f, -1.0f),  mork::vec2f(0.0f, 0.0f)},

            {mork::vec3f(-1.0f, -1.0f,  1.0f),  mork::vec2f(0.0f, 0.0f)},
            {mork::vec3f( 1.0f, -1.0f,  1.0f),  mork::vec2f(1.0f, 0.0f)},
            {mork::vec3f( 1.0f,  1.0f,  1.0f),  mork::vec2f(1.0f, 1.0f)},
            {mork::vec3f( 1.0f,  1.0f,  1.0f),  mork::vec2f(1.0f, 1.0f)},
            {mork::vec3f(-1.0f,  1.0f,  1.0f),  mork::vec2f(0.0f, 1.0f)},
            {mork::vec3f(-1.0f, -1.0f,  1.0f),  mork::vec2f(0.0f, 0.0f)},

            {mork::vec3f(-1.0f,  1.0f,  1.0f),  mork::vec2f(1.0f, 0.0f)},
            {mork::vec3f(-1.0f,  1.0f, -1.0f),  mork::vec2f(1.0f, 1.0f)},
            {mork::vec3f(-1.0f, -1.0f, -1.0f),  mork::vec2f(0.0f, 1.0f)},
            {mork::vec3f(-1.0f, -1.0f, -1.0f),  mork::vec2f(0.0f, 1.0f)},
            {mork::vec3f(-1.0f, -1.0f,  1.0f),  mork::vec2f(0.0f, 0.0f)},
            {mork::vec3f(-1.0f,  1.0f,  1.0f),  mork::vec2f(1.0f, 0.0f)},

            {mork::vec3f(1.0f,  1.0f,  1.0f),  mork::vec2f(1.0f, 0.0f)},
            {mork::vec3f( 1.0f,  1.0f, -1.0f),  mork::vec2f(1.0f, 1.0f)},
            {mork::vec3f(  1.0f, -1.0f, -1.0f),  mork::vec2f(0.0f, 1.0f)},
            {mork::vec3f( 1.0f, -1.0f, -1.0f),  mork::vec2f(0.0f, 1.0f)},
            {mork::vec3f( 1.0f, -1.0f,  1.0f),  mork::vec2f(0.0f, 0.0f)},
            {mork::vec3f( 1.0f,  1.0f,  1.0f),  mork::vec2f(1.0f, 0.0f)},

            {mork::vec3f(-1.0f, -1.0f, -1.0f),  mork::vec2f(0.0f, 1.0f)},
            {mork::vec3f( 1.0f, -1.0f, -1.0f),  mork::vec2f(1.0f, 1.0f)},
            {mork::vec3f( 1.0f, -1.0f,  1.0f),  mork::vec2f(1.0f, 0.0f)},
            {mork::vec3f( 1.0f, -1.0f,  1.0f),  mork::vec2f(1.0f, 0.0f)},
            {mork::vec3f(-1.0f, -1.0f,  1.0f),  mork::vec2f(0.0f, 0.0f)},
            {mork::vec3f(-1.0f, -1.0f, -1.0f),  mork::vec2f(0.0f, 1.0f)},

            {mork::vec3f(-1.0f,  1.0f, -1.0f),  mork::vec2f(0.0f, 1.0f)},
            {mork::vec3f( 1.0f,  1.0f, -1.0f),  mork::vec2f(1.0f, 1.0f)},
            {mork::vec3f( 1.0f,  1.0f,  1.0f),  mork::vec2f(1.0f, 0.0f)},
            {mork::vec3f( 1.0f,  1.0f,  1.0f),  mork::vec2f(1.0f, 0.0f)},
            {mork::vec3f(-1.0f,  1.0f,  1.0f),  mork::vec2f(0.0f, 0.0f)},
            {mork::vec3f(-1.0f,  1.0f, -1.0f),  mork::vec2f(0.0f, 1.0f)}
        };       
        
       
        vao.bind();
        vb.bind();
        vb.setData(verts);
        vb.setAttributes();
        vb.unbind();
        vao.unbind();


        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        tex1.loadTexture("textures/container.jpg", false);
        tex2.loadTexture("textures/awesomeface.png", true);
        prog.use();
        prog.getUniform("texture1").set(0);
        prog.getUniform("texture2").set(1);

        glEnable(GL_DEPTH_TEST);

    }

    ~App() {
    }

    virtual void redisplay(double t, double dt) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    

         // draw our first triangle
        double timeValue = timer.getTime();
      
        mork::mat4f model = mork::mat4f::rotatex(timeValue);
        model = mork::mat4f::rotatey(0.1*timeValue)*model;
        
        mork::mat4f view = mork::mat4f::translate(mork::vec3f(0.0, 0.0, sin(timeValue/0.5)*2.0-7.0));

        float aspect = this->getWidth()/this->getHeight();
        mork::mat4f proj = mork::mat4f::perspectiveProjection(radians(45.0), aspect, 0.1, 100); 

        prog.getUniform("transform").set(proj*view*model);
        prog.use();
        vao.bind();
        glActiveTexture(GL_TEXTURE0 + 0);
        tex1.bind();
        glActiveTexture(GL_TEXTURE0 + 1);
        tex2.bind();
        glDrawArrays(GL_TRIANGLES, 0, 36); 
   
        GlfwWindow::redisplay(t, dt);
  
    }

    virtual void reshape(int x, int y)
    {
        // TODO:
        // Move to new Framebuffer class
        glViewport(0, 0, x, y);
        GlfwWindow::reshape(x, y);
        idle(false);
    }

    virtual bool specialKey(key k, modifier m, int x, int y)
    {
        switch (k) {
        case KEY_ESCAPE:
                shouldClose();             
            return true;
        default:
            break;
        }
        return false;
    }
private:
    mork::Timer timer;
    
    mork::VertexBuffer<mork::vertex_pos_uv> vb;
    mork::GPUBuffer<unsigned int, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW> ib;

    mork::Texture<2> tex1, tex2;


    mork::VertexArrayObject vao;
    mork::Program prog;    
         
    unsigned int texture;
 
};


int main(int argc, char** argv) {

    mork::Timer timer;

    string exename(argv[0]);

    //string inputFile;
    //int     verbose = 0;
    cxxopts::Options options(argv[0], "Space Simulator Client\n(c) 2017 Lars Flæten");
    options.add_options()
        ("h,help", "Print help")
        ;


    auto result = options.parse(argc, argv);

    if(result.count("help"))
    {
        cout << options.help({""}) << endl;
        return 0;
    }
    mork::info_logger("Starting client application..");   
    timer.start();

    App app;

    app.start();
   
    timer.end();
    mork::info_logger("Client application shutdown.");   
    mork::info_logger("Took: ", timer.getDuration(), "s"); 

    return 0;
}



