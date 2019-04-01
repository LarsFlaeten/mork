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

using namespace std;

static std::string window_title = "Normals";

class TextBox {
    public:
        TextBox(int width, int height) : 
            fb(width, height), 
            dirty(true), 
            text(""),
            ortho(mork::mat4f::orthoProjection(width, 0.0f, height, 0.0f, -1.0f, 1.0f)) 
        {
            // Set background as initially transparent
            fb.setClearColor(mork::vec4f(0.0f, 0.0f, 0.0f, 0.0f));
        }
        
        void setText(const std::string& _text) {
            if(_text != text) {
                text = _text;
                dirty = true;
            }
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
            return fb.getColorBuffer();
        }

    private:
        bool dirty;
        std::string text;
        mork::Framebuffer fb;
        mork::mat4f ortho; 

};


using VN = mork::vertex_pos_norm_uv;
using VTBN = mork::vertex_pos_norm_tang_bitang_uv;

class App : public mork::GlfwWindow {
public:
    App()
            : mork::GlfwWindow(mork::Window::Parameters().size(800,600).name(window_title)),
               prog(330, "shaders/ex11.glsl"),
               font(mork::Font::createFont("resources/fonts/LiberationSans-Regular.ttf", 48)),
               textBox(800, 600),
               fsQuad(mork::MeshHelper<VN>::PLANE()),
               quadProg(330, "shaders/quadShader.glsl"),
               normalProg(330, "shaders/normalShader.glsl"),
               tbnProg(330, "shaders/TBNShader.glsl"),
               showNormals(false),
               showTangents(false),
               showHelp(false),
               showlines(false)
    {
        up = false;
        down = false;
        left = false;
        right = false;
        double radius = 1737.0;
        radius = 1.0;
 
        
        auto moonMesh1 = mork::MeshHelper<VTBN>::SPHERE(radius, radius, radius, 40, 80); 
        //auto moonMesh1 = mork::MeshHelper<VTBN>::PLANE();//(radius, radius, radius, 4, 8); 
        
        mork::Material moonMat1;
        moonMat1.diffuseLayers.push_back(mork::TextureLayer(mork::Texture<2>::fromFile("textures/moon-4k.png", true)));
        moonMat1.normalLayers.push_back(mork::TextureLayer(mork::Texture<2>::fromFile("textures/moon_normal.jpg", true)));
        auto moon1 = std::make_unique<mork::Model>("moon1", std::move(moonMesh1), std::move(moonMat1));


        // Material 2 - texture only
        mork::Material moonMatTexOnly;
        moonMatTexOnly.diffuseLayers.push_back(mork::TextureLayer(mork::Texture<2>::fromFile("textures/moon-4k.png", true)));
        moon1->addMaterial(std::move(moonMatTexOnly));

        // Material 3 - normal map only
        mork::Material moonMatNormOnly;
        moonMatNormOnly.diffuseColor = mork::vec3f(1.0, 1.0, 1.0);
        moonMatNormOnly.normalLayers.push_back(mork::TextureLayer(mork::Texture<2>::fromFile("textures/moon_normal.jpg", true)));
        moon1->addMaterial(std::move(moonMatNormOnly));



        moon1->setLocalToParent(mork::mat4d::translate(mork::vec3d(5*radius, 0, 0))*mork::mat4d::rotatez(radians(180.0)));
        //moon1->setLocalToParent(mork::mat4d::translate(mork::vec3d(5*radius, 0, 0))*mork::mat4d::rotatex(radians(90.0))*mork::mat4d::rotatey(radians(-90.0)));
        scene.getRoot().addChild(std::move(moon1));

             
        scene.getCamera().setPosition(mork::vec4d(0, 0, 0, 1));
        scene.getCamera().lookAt(mork::vec3d(1,0,0), mork::vec3d(0, 0, 1));
        scene.getCamera().setClippingPlanes(0.1,100.0*radius);

        mork::GlfwWindow::waitForVSync(false);


    }

    ~App() {
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
        // Adjust camera:
        auto& camera = scene.getCamera();
        if( up || down || left || right ) {
            mork::vec3d axis = mork::vec3d::ZERO;
            double angle = 0.0;
            if( up || down)
                axis = mork::vec3d(1,0,0);
            if( right || left)
                axis = mork::vec3d(0,1,0);
            if( up || left)
                angle = 30.0;
            if( down || right)
                angle = -30.0;
            mork::mat3d mat = camera.getRotation();
            axis = mat*axis; // Transform axis to global
            camera.setRotation(mork::quatd(axis, radians(angle*this->getDt())).toMat4().mat3x3()*mat);
        }

        if(keys.count('W')) {
            // Move camera foward 2.0 units / s
            mork::vec3d pos = camera.getPosition().xyz();
            pos += camera.getWorldForward()*this->getDt()*2.0;
            camera.setPosition(mork::vec4d(pos));
        } else if(keys.count('S')) {
            // Move camera backwards 2.0 units / s
            mork::vec3d pos = camera.getPosition().xyz();
            pos -= camera.getWorldForward()*this->getDt()*2.0;
            camera.setPosition(mork::vec4d(pos));
        } else if(keys.count('A')) {
            // Move camera left 2.0 units / s
            mork::vec3d pos = camera.getPosition().xyz();
            pos -= camera.getWorldRight()*this->getDt()*2.0;
            camera.setPosition(mork::vec4d(pos));
        } else if(keys.count('D')) {
            // Move camera right 2.0 units / s
            mork::vec3d pos = camera.getPosition().xyz();
            pos += camera.getWorldRight()*this->getDt()*2.0;
            camera.setPosition(mork::vec4d(pos));
        }
 
        
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
      
           
        prog.use();
        dirLight.set(prog, "dirLight");
 
        spotLight.set(prog, "spotLight");
       
        pointLight.set(prog, "pointLight");

        scene.draw(prog);
        
        if(showNormals) {       
            normalProg.use(); 
            normalProg.getUniform("scale").set(0.1f);
       
            scene.draw(normalProg);
        }
        if(showTangents) {       
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
            info << "Cam dir: " << camera.getWorldForward() << ", up: " << camera.getWorldUp() << "\n";
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
            case mork::EventHandler::key::KEY_UP:
                up = true;
                down = false;
                left = false;
                right = false;
                handled =  true;
                break;
            case mork::EventHandler::key::KEY_DOWN:
                up = false;
                down = true;
                left = false;
                right = false;
                handled =  true;
                break;
            case mork::EventHandler::key::KEY_RIGHT:
                up = false;
                down = false;
                left = false;
                right = true;
                handled =  true;
                break;
            case mork::EventHandler::key::KEY_LEFT:
                up = false;
                down = false;
                left = true;
                right = false;
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
            case mork::EventHandler::key::KEY_UP:
                up = false;
                handled =  true;
                break;
            case mork::EventHandler::key::KEY_DOWN:
                down = false;
                handled =  true;
                break;
            case mork::EventHandler::key::KEY_RIGHT:
                right = false;
                handled =  true;
                break;
            case mork::EventHandler::key::KEY_LEFT:
                left = false;
                handled =  true;
                break;
            default:
                break;
        }
        return false;
    }


    virtual bool keyTyped(unsigned char c, modifier m, int x, int y)
    {
        keys.insert(c);

        if(keys.count('N'))
            showNormals = !showNormals;
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

    virtual bool mouseMotion(int x, int y) {
        return false;
    }

    virtual bool mousePassiveMotion(int x, int y) {

        return false;
    }

    virtual void fpsUpdatedEvent() {
        std::stringstream os;
        os << window_title + " (FPS: " << this->getFps() << ")";
        this->setWindowTitle(os.str());

    }
private:
    bool up, down, left, right;
    bool showNormals;
    bool showlines;
    bool showTangents;
    bool showHelp;
    mork::Timer timer;

    mork::Font font;

    mork::Program prog;
    mork::Program quadProg;
    mork::Program normalProg;
    mork::Program tbnProg;


    //mork::BasicMesh mesh;
    //mork::TBNMesh plane;
    std::set<char> keys;
 
    mork::Scene scene;

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

    App app;

    app.start();
   
    timer.end();
    mork::info_logger("Client application shutdown.");   
    mork::info_logger("Took: ", timer.getDuration(), "s"); 

    return 0;
}


