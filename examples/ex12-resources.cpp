#include <iostream>
#include <stdexcept>
#include <vector>
#include <sstream>
#include <set>
#include <cxxopts.hpp>
#include <random>

#include "mork/ui/GlfwWindow.h"
#include "mork/core/Log.h"
#include "mork/render/Font.h"
#include "mork/scene/Scene.h"
#include "mork/scene/SceneNode.h"
#include "mork/render/Light.h"
#include "mork/math/quat.h"
#include "mork/render/Mesh.h"
#include "mork/render/Material.h"
#include "mork/util/ModelImporter.h"
#include "mork/render/Framebuffer.h"
#include "mork/core/stb_image.h"
#include "mork/util/MeshUtil.h"

#include "mork/util/BBoxDrawer.h"
#include "mork/resource/ResourceManager.h"
#include "mork/resource/ResourceFactory.h"

using namespace std;

class TextBox {
    public:
        TextBox(int width, int height) : 
            fb(width, height),
            colorBuffer(width, height, GL_RGBA, false), 
            dirty(true), 
            text(""),
            ortho(mork::mat4f::orthoProjection(width, 0.0f, height, 0.0f, -1.0f, 1.0f)) 
        {
            // Set background as initially transparent
            fb.setClearColor(mork::vec4f(0.0f, 0.0f, 0.0f, 0.0f));
            fb.attachColorBuffer(colorBuffer);
        }
        
        void setText(const std::string& _text) {
            if(_text != text) {
                text = _text;
                dirty = true;
            }
        }
        
        void setSize(int width, int height) {
            ortho = mork::mat4f::orthoProjection(width, 0.0f, height, 0.0f, -1.0f, 1.0f);
            fb.setSize(mork::vec2i(width, height));            
            dirty = true;
        }


        void drawToBuffer(mork::Font& font) {
            if(dirty) {
                fb.bind();
                fb.clear();
                auto s = fb.getSize();
                font.drawText(text, 5, s.y-font.getYMax(18)-5, 18, mork::vec3f(1.0, 1.0, 1.0), ortho);
                dirty = false;       

            }
        }

        const mork::Texture<2>& getColorBuffer() const {
            return colorBuffer;
        }

    private:
        bool dirty;
        std::string text;
        mork::Framebuffer fb;
        mork::Texture<2> colorBuffer;
        mork::mat4f ortho; 

};


using VN = mork::vertex_pos_norm_uv;
using VTBN = mork::vertex_pos_norm_tang_bitang_uv;

class App : public mork::GlfwWindow {
public:
    App(mork::ResourceManager& _manager)
            :   mork::GlfwWindow(mork::ResourceFactory<mork::Window::Parameters>::getInstance().create(_manager,"window1")),
                manager(_manager),
                progs(mork::ResourceFactory<mork::ProgramPool>::getInstance().create(manager,"programPool1")),
                scene(mork::ResourceFactory<mork::Scene>::getInstance().create(_manager,"scene1")),
                font(mork::Font::createFont("resources/fonts/LiberationSans-Regular.ttf", 48)),
                textBox(this->getWidth(), this->getHeight()),
                fsQuad(mork::MeshHelper<VN>::PLANE()),
                showNormals(false),
                showTangents(false),
                showHelp(false),
                showlines(false)
    {
        mork::GlfwWindow::waitForVSync(false);
    }

    ~App() {
    }

    void reloadResources() {
           const auto& r1 = manager.getResource("scene1");
           if(r1.needUpdate()) {
               auto fp = r1.getFilePath();
                mork::info_logger("Updating scene resource");
                manager.removeResource("scene1");
                manager.loadResource(fp, "scene1");
                scene = std::move(mork::ResourceFactory<mork::Scene>::getInstance().create(manager,"scene1"));
            } else {
                mork::info_logger("Scene resource does not need update");
            }


            const auto& r2 = manager.getResource("programPool1");
            if(r2.needUpdate()) {
                auto fp = r2.getFilePath();
                mork::info_logger("Updating program pool resource");
                manager.removeResource("programPool1");
                manager.loadResource(fp, "programPool1");
                progs = std::move(mork::ResourceFactory<mork::ProgramPool>::getInstance().create(manager,"programPool1"));

            } else {
                mork::info_logger("Program pool resource does not need update");
            }

    }

    virtual void redisplay(double t, double dt) {
       
        // Framebuffer ops 
        mork::Framebuffer::getDefault().bind();
        mork::Framebuffer::getDefault().clear();

        // State group:
        
        // We are now rending large but sparse objects at great distances.
        // disable depth test for these..
        glDisable(GL_DEPTH_TEST);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        if(showlines)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


        glEnable(GL_CULL_FACE);




        double timeValue = timer.getTime();
     
        // Update sceme:
        scene.update();
    
        // Prepare rendering
        mork::DirLight dirLight;
        dirLight.setDirection(mork::vec3d(1,1,0).normalize());
        dirLight.setAmbientColor(mork::vec3d(0.10, 0.1, 0.1));
        dirLight.setDiffuseColor(mork::vec3d(1.0, 1.0, 1.0));
        dirLight.setSpecularColor(mork::vec3d(1.0, 1.0, 1.0));



        
        // Prepare for normal rendering:
        glDepthMask(GL_TRUE);
 
        mork::SpotLight spotLight;
        spotLight.setColor(mork::Light::NO_LIGHT);
        
        mork::PointLight pointLight;
        pointLight.setColor(mork::Light::NO_LIGHT);
      
           
        auto& prog = progs.at("prog");
        prog.use();
        dirLight.set(prog, "dirLight");
 
        spotLight.set(prog, "spotLight");
       
        pointLight.set(prog, "pointLight");

        scene.draw(prog);
        
        if(showNormals) {       
            auto& normalProg = progs.at("normalProg");
            normalProg.use(); 
            normalProg.getUniform("scale").set(0.1f);
       
            scene.draw(normalProg);
        }
        if(showTangents) {       
            auto& tbnProg = progs.at("tbnProg");
            tbnProg.use(); 
            tbnProg.getUniform("scale").set(0.2f);
       
            scene.draw(tbnProg);
        }
        // Draw 2D Text 
        
        // State group, only diff is shown
        if(showHelp) {
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

          
             
            mork::mat4f ortho = mork::mat4f::orthoProjection(this->getWidth(), 0.0f, this->getHeight(), 0.0f, -1.0f, 1.0f);

            std::stringstream info;
            info << "Multiline text:\n";
            info << "FPS: " << this->getFps() << ", frametime: " << this->getFrameTime() << "s\n";
            auto& camera = scene.getCamera();
            info << "Cam dir: " << camera.getWorldForward() << ", up: " << camera.getWorldUp() << ", world pos: " << camera.getWorldPosition() << ", local pos: " << camera.getPosition() << "\nCamera Mode: " << (camera.getMode() == mork::Camera::Mode::FREE ? "FREE" : "ORBIT")  <<"\n";
            info << "Moon pos: " << scene.getRoot().getChild("moon1").getLocalToWorld().translation() << "\n";
            info << "Key menu:\n";
            info << "\tPress [H] to toggle this text (on = " << showHelp << ")\n";
            info << "\tPress [N] to toggle normals (on = " << showNormals << ")\n";
            info << "\tPress [T] to toggle tangents (on = " << showTangents << ")\n";
            info << "\tPress [M] to toggle wireframe\n";
            info << "\tPress [ESC] to quit\n"; 
            info << "Keys: ";
            for(auto c: keys)
                info << c << "[" << (int)c << "], ";
                    

            textBox.setText(info.str());
            textBox.drawToBuffer(font);

            // State changes
            glEnable(GL_BLEND); 
            mork::Framebuffer::getDefault().bind();
           
            auto& quadProg = progs.at("quadProg"); 
            quadProg.use(); 
            quadProg.getUniform("tex").set(0);
            textBox.getColorBuffer().bind(0);
            fsQuad.draw();
        }

        GlfwWindow::redisplay(t, dt);
        
    }

    virtual void reshape(int x, int y)
    {
        // Resizes default framebuffer (will also sort out glViewport
        mork::Framebuffer::getDefault().setSize(mork::vec2i(x, y));
        GlfwWindow::reshape(x, y);

        scene.getCamera().setAspectRatio(static_cast<double>(x), static_cast<double>(y));
            
        //textBox.setSize(x, y);
        
        idle(false);
    }

    virtual bool specialKey(key k, modifier m, int x, int y)
    {
        bool handled = false;
        switch (k) {
            case mork::EventHandler::key::KEY_ESCAPE:
                shouldClose();             
                handled =  true;
                break;
           default:
                break;
        }
        return false;
    }
    
    virtual bool specialKeyReleased(key k, modifier m, int x, int y)
    {
        bool handled = false;
        switch (k) {
            case mork::EventHandler::key::KEY_ESCAPE:
                shouldClose();             
                handled =  true;
                break;
           case mork::EventHandler::key::KEY_F5:
                reloadResources();
                handled = true;
                break;
            default:
                break;
        }
        return handled;
    }


    virtual bool keyTyped(unsigned char c, modifier m, int x, int y)
    {
        keys.insert(c);

        if(keys.count('N'))
            showNormals = !showNormals;
        if(keys.count('R'))
            std::cout << manager << std::endl;
       
        if(keys.count('M'))
            showlines = !showlines;
        if(keys.count('H'))
            showHelp = !showHelp;
        if(keys.count('T'))
            showTangents = !showTangents;
        if(keys.count('1')) {
            auto& moon_node = scene.getRoot().getChild("moon1");
            auto& moon = dynamic_cast<mork::Model&>(moon_node);
            moon.getMesh(0).setMaterialIndex(0);
        }
        if(keys.count('2')) {
            auto& moon_node = scene.getRoot().getChild("moon1");
            auto& moon = dynamic_cast<mork::Model&>(moon_node);
            moon.getMesh(0).setMaterialIndex(1);
        }
        if(keys.count('3')) {
            auto& moon_node = scene.getRoot().getChild("moon1");
            auto& moon = dynamic_cast<mork::Model&>(moon_node);
            moon.getMesh(0).setMaterialIndex(2);
        }
        return true;
    }

    virtual bool keyReleased(unsigned char c, modifier m, int x, int y) {
        keys.erase(c);

        return true;
    }

    virtual bool mouseClick(button b, state s, modifier m, int x, int y) {
        previous_mouse_x = x;
        previous_mouse_y = y;
    }

    // While button is clicked
    virtual bool mouseMotion(int x, int y) {
   	    constexpr double kScale = 500.0;
        mork::Camera& cam = scene.getCamera();
        double az_rad = cam.getAzimuth();
        double el_rad = cam.getElevation();


  	    el_rad -= (previous_mouse_y - y) / kScale;
    	az_rad += (previous_mouse_x - x) / kScale;

        cam.setAzimuth(az_rad);
        cam.setElevation(el_rad);
  		
  		previous_mouse_x = x;
  		previous_mouse_y = y;
        return true;
    }

    virtual bool mousePassiveMotion(int x, int y) {

        return false;
    }
    
    virtual bool mouseWheel(wheel b, modifier m, int x, int y) {
        mork::Camera& cam = scene.getCamera();
        double distance = cam.getDistance(); 
        if (b==mork::EventHandler::WHEEL_UP ) {
            distance *= 1.05;
        } else {
            distance /= 1.05;
        }
        cam.setDistance(distance);
    }


    virtual void fpsUpdatedEvent() {
        std::stringstream os;
        os << this->getTitle() + " (FPS: " << this->getFps() << ")";
        this->setWindowTitle(os.str());

    }
private:
    bool showNormals;
    bool showlines;
    bool showTangents;
    bool showHelp;
    
    int previous_mouse_x;
    int previous_mouse_y;
 
    mork::ResourceManager& manager;


    mork::Timer timer;

    mork::Font font;

    mork::ProgramPool progs;
    mork::Scene scene;


    //mork::Program prog;
    //mork::Program quadProg;
    //mork::Program normalProg;
    //mork::Program tbnProg;

   //mork::BasicMesh mesh;
    //mork::TBNMesh plane;
    std::set<char> keys;
 
    TextBox textBox;

    mork::Mesh<VN>  fsQuad;

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
    mork::ResourceManager manager("ex12.json");

    App app(manager);

    app.start();
   
    timer.end();
    mork::info_logger("Client application shutdown.");   
    mork::info_logger("Took: ", timer.getDuration(), "s"); 

    return 0;
}


