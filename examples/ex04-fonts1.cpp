#include <iostream>
#include <stdexcept>
#include <vector>

#include <cxxopts.hpp>

#include "mork/ui/GlfwWindow.h"
#include "mork/core/Log.h"
#include "mork/render/Program.h"
#include "mork/render/VertexArrayObject.h"
#include "mork/render/FontEngine.h"
using namespace std;



class App : public mork::GlfwWindow {
public:
    App()
            : mork::GlfwWindow(mork::Window::Parameters().size(800,600))
            //prog(std::string(vertexShaderSource), std::string(fragmentShaderSource))
    {
        mork::FontEngine::init();

        font1 = mork::FontEngine::getFont("Ubuntu");
        font2 = mork::FontEngine::getFont("Liberation Sans");
        font3 = mork::FontEngine::getFont("Liberation Mono");

    }

    ~App() {
    }

    virtual void redisplay(double t, double dt) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    

        
        mork::mat4f proj = mork::mat4f::orthoProjection(this->getWidth(), 0.0f, this->getHeight(), 0.0f, -1.0f, 1.0f);
        mork::FontEngine::getProgram().use();
        mork::FontEngine::getProgram().getUniform("projection").set(proj);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        font1.drawText("Ubuntu", 25.0f, 25.0f, 1.0f, mork::vec3f(1.0, 1.0, 1.0));
        font2.drawText("Liberation Sans", 400, 300, 0.4, mork::vec3f(1.0, 0.5, 0.5));
        font3.drawText("Liberation Mono", 50, 500, 0.6, mork::vec3f(0.0, 0.5, 1.0));
   
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
    mork::Font  font1, font2, font3;


    //mork::Program prog;    
 
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


