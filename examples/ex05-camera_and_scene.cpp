#include <iostream>
#include <stdexcept>
#include <vector>
#include <sstream>

#include <cxxopts.hpp>

#include "mork/ui/GlfwWindow.h"
#include "mork/core/Log.h"
#include "mork/render/Program.h"
#include "mork/render/Font.h"
#include "mork/scene/Scene.h"
#include "mork/scene/SceneNode.h"

using namespace std;

static std::string window_title = "Camera and Scene demo";

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

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 texCoord;\n"
    "\n"
    "uniform sampler2D texture1;\n"
    "uniform sampler2D texture2;\n"
    "uniform vec4 colorMask;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   //FragColor = texture(texture1, texCoord) + 0.0*texture(texture2, texCoord);\n"
    "   FragColor = colorMask*vec4(vec3(mix(texture(texture1, texCoord), texture(texture2, texCoord), 0.2)), 1.0);\n"
    "}\n\0";


class App : public mork::GlfwWindow {
public:
    App()
            : mork::GlfwWindow(mork::Window::Parameters().size(800,600).name(window_title)),
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


  
        for(int i = 1; i <= 10; ++i) {
            nodes[i] = std::make_shared<mork::SceneNode>();
            scene.getRoot().addChild(nodes[i]);
        }

        camera = std::make_shared<mork::Camera>();
        camera->setFOV(radians(45.9));
        camera->setPosition(mork::vec4d(-10, 0, 0, 1));
        camera->lookAt(mork::vec3d(1,0,0), mork::vec3d(0, 0, 1));
        camera->setReference(nodes[5]);
        scene.addCamera(camera);



        waitForVSync(false);
    }

    ~App() {
    }

    virtual void redisplay(double t, double dt) {
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    

        double timeValue = timer.getTime();
     
        // Update sceme:
        for(auto& a : nodes) {
            int i = a.first;
            a.second->setLocalToParent(
                mork::mat4d::translate(mork::vec3d(10.0, 2*i-11, 0))*mork::mat4d::rotatez(0.01*i*timeValue)*mork::mat4d::rotatey(0.1*i*timeValue)
                );
        }
        scene.update();


       
        mork::mat4d view = camera->getViewMatrix();

        mork::mat4d proj = camera->getProjectionMatrix(); 


        // draw model:
        for(auto& a : nodes) {
            mork::mat4d model = a.second->getLocalToWorld();
            mork::mat4f trans = (proj*view*model).cast<float>();
            prog.getUniform("transform").set(trans);
            if(camera->getReference() == a.second)
                prog.getUniform("colorMask").set(mork::vec4f(1.0, 0.0, 0.0, 1.0));
            else
                prog.getUniform("colorMask").set(mork::vec4f(1.0, 1.0, 1.0, 1.0));
            
            
            prog.use();
            vao.bind();
            glActiveTexture(GL_TEXTURE0 + 0);
            tex1.bind();
            glActiveTexture(GL_TEXTURE0 + 1);
            tex2.bind();
            glDrawArrays(GL_TRIANGLES, 0, 36); 
        }

        // Draw 2D Text 
        glDisable(GL_DEPTH_TEST);
        
        





        GlfwWindow::redisplay(t, dt);
  
    }

    virtual void reshape(int x, int y)
    {
        // TODO:
        // Move to new Framebuffer class
        glViewport(0, 0, x, y);
        GlfwWindow::reshape(x, y);

        camera->setAspectRatio(static_cast<double>(x), static_cast<double>(y));
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

    virtual bool keyTyped(unsigned char c, modifier m, int x, int y)
    {
        keys[c] = true;

        return false;
    }

    virtual bool keyReleased(unsigned char c, modifier m, int x, int y) {
        keys.erase(c);

        return false;
    }


    virtual void fpsUpdatedEvent() {
        std::stringstream os;
        os << window_title + " (FPS: " << this->getFps() << ")";
        this->setWindowTitle(os.str());

    }
private:
    mork::Timer timer;

    std::map<char, bool> keys;

    mork::VertexBuffer<mork::vertex_pos_uv> vb;
    mork::GPUBuffer<unsigned int, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW> ib;

    mork::Texture<2> tex1, tex2;


    mork::VertexArrayObject vao;
    mork::Program prog;    
 
    mork::Scene scene;

    map<int, std::shared_ptr<mork::SceneNode> > nodes;

    std::shared_ptr<mork::Camera> camera;
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


