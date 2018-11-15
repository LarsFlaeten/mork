#include <iostream>
#include <stdexcept>
#include <vector>
#include <sstream>

#include <cxxopts.hpp>

#include "mork/ui/GlfwWindow.h"
#include "mork/core/Log.h"
#include "mork/render/Program.h"
#include "mork/render/VertexArrayObject.h"
#include "mork/render/Font.h"
using namespace std;

static std::string window_title = "Fonts demo";

class App : public mork::GlfwWindow {
public:
    App()
            : mork::GlfwWindow(mork::Window::Parameters().size(800,600).name(window_title))
            //prog(std::string(vertexShaderSource), std::string(fragmentShaderSource))
    {

        font1 = mork::Font::createFont("resources/fonts/Ubuntu-R.ttf");
        font2 = mork::Font::createFont("resources/fonts/LiberationMono-Regular.ttf");
        font3 = mork::Font::createFont("resources/fonts/LiberationSans-Regular.ttf");
        
        showText = true;
        blend = true;
        waitForVSync(false);
    }

    ~App() {
    }

    virtual void redisplay(double t, double dt) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    

        if(showText) {
            mork::mat4f proj = mork::mat4f::orthoProjection(this->getWidth(), 0.0f, this->getHeight(), 0.0f, -1.0f, 1.0f);
            if(blend) {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            } else
                glDisable(GL_BLEND);
                
            font1.drawText("Ubuntu", 25.0f, 25.0f, 48, mork::vec3f(1.0, 1.0, 1.0), proj);
            font2.drawText("Liberation Sans", 400, 300, 20, mork::vec3f(1.0, 0.5, 0.5), proj);
            font3.drawText("Liberation Mono\n\tTabbed text", 50, 300, 26, mork::vec3f(0.0, 0.5, 1.0), proj);
        
            

            std::stringstream info;
            info << "Multiline text:\n";
            info << "FPS: " << this->getFps() << ", frametime: " << this->getFrameTime() << "s\n";
            info << "Key menu:(entries are tabbed)\n";
            info << "\tPress [i] to toggle text\n";
            info << "\tPress [b] to toggle blending\n";
            info << "\tPress [ESC] to quit\n"; 
            info << "Keys: ";
            for(auto c: keys)
                info << c.first << "[" << (int)c.first << "], ";



            font1.drawText(info.str(), 25.0f, this->getHeight()-20, 16, mork::vec3f(1.0, 0.0, 0.0), proj);
        }
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

    virtual bool keyTyped(unsigned char c, modifier m, int x, int y)
    {
        keys[c] = true;

        switch (c) {
        case 'I':
            showText = !showText;             
            return true;
        case 'B':
            blend = !blend;
            return true;
        default:
            break;
        }
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
    mork::Font  font1, font2, font3;

    bool showText;
    bool blend;
    std::map<char, bool> keys;
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


