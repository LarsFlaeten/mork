#include <iostream>
#include <stdexcept>
#include <vector>
#include <sstream>
#include <set>
#include <cxxopts.hpp>

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

#include "mork/util/BBoxDrawer.h"

using namespace std;

static std::string window_title = "Model 01";

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aNorm;\n"
    "layout (location = 2) in vec2 aUv;\n"
    "out vec2 texCoord;\n"
    "out vec3 fragPos;\n"
    "out vec3 normal;\n"
    "uniform mat4 projection;\n"
    "uniform mat4 view;\n"
    "uniform mat4 model;\n"
    "uniform mat3 normalMat;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "   fragPos = vec3(model * vec4(aPos, 1.0));\n"
    "   texCoord = aUv;\n"
    "   normal = normalMat * aNorm;\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "#include \"shaders/materials.glhl\"\n"
    "#include \"shaders/lights.glhl\"\n"
    "out vec4 FragColor;\n"
    "in vec2 texCoord;\n"
    "in vec3 fragPos;\n"
    "in vec3 normal;\n"
    "\n"
    "\n"
    "uniform PointLight pointLight;\n"
    "uniform DirLight dirLight;\n"
    "uniform SpotLight spotLight;\n"
    "uniform Material material;\n"
    "uniform vec3 viewPos;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   // Base color contribution:\n"
    "   vec3 ambientColor = evaluateTextureLayers(material.ambientColor, material.ambientLayers, material.numAmbientLayers, texCoord);\n"
    "   vec3 diffuseColor = evaluateTextureLayers(material.diffuseColor, material.diffuseLayers, material.numDiffuseLayers, texCoord);\n"
    "   vec3 specularColor = evaluateTextureLayers(material.specularColor, material.specularLayers, material.numSpecularLayers, texCoord);\n"
    "   vec3 emissiveColor = evaluateTextureLayers(material.emissiveColor, material.emissiveLayers, material.numEmissiveLayers, texCoord);\n"
    "\n"
    "   // Calculate light contribution:\n"
    "   vec3 lightResult = vec3(0.0, 0.0, 0.0);\n"
    "   vec3 viewDir = normalize(viewPos - fragPos);\n"
    "   lightResult += CalcPointLight(ambientColor, diffuseColor, specularColor, pointLight, normal, fragPos, viewDir, material);\n"
    "   lightResult += CalcDirLight(ambientColor, diffuseColor, specularColor, dirLight, normal, viewDir, material);\n"
    "   lightResult += CalcSpotLight(ambientColor, diffuseColor, specularColor, spotLight, normal, fragPos, viewPos, material);\n"
    "\n"
    "   vec3 total = emissiveColor + lightResult;\n"
    "\n"
    "   FragColor = vec4((total), 1.0);\n"
    "}\n\0";

const char *fragmentShaderSource2 = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 texCoord;\n"
    "in vec3 fragPos;\n"
    "in vec3 normal;\n"
    "\n"
    "uniform vec3 color;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(color, 1.0);\n"
    "}\n\0";



class App : public mork::GlfwWindow {
public:
    App()
            : mork::GlfwWindow(mork::Window::Parameters().size(800,600).name(window_title)),
               prog(std::string(vertexShaderSource), std::string(fragmentShaderSource)),
               font(mork::Font::createFont("resources/fonts/LiberationSans-Regular.ttf", 48))
    {
        up = false;
        down = false;
        left = false;
        right = false;

         
        //model= mork::ModelImporter::loadModel("models/nff/", "sphere.nff"); 
        std::unique_ptr<mork::SceneNode> model = std::make_unique<mork::Model>(mork::ModelImporter::loadModel("models/nanosuit/", "nanosuit.obj", "model")); 
        model->setLocalToParent(mork::mat4d::translate(mork::vec3d(0,0,-7))*mork::mat4d::rotatez(radians(-90.0))*mork::mat4d::rotatex(radians(90.0)));
        //model = mork::ModelImporter::loadModel("/home/lars/workspace/assimp/test/models-nonbsd/OBJ/", "segment.obj"); 
        scene.getRoot().addChild(std::move(model));        

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                

        camera = std::make_shared<mork::Camera>();
        camera->setFOV(radians(45.0));
        camera->setPosition(mork::vec4d(-10, 0, 0, 1));
        camera->lookAt(mork::vec3d(1,0,0), mork::vec3d(0, 0, 1));
        scene.addCamera(camera);


        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        waitForVSync(false);
    }

    ~App() {
    }

    virtual void redisplay(double t, double dt) {


        glEnable(GL_DEPTH_TEST);
        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    

        double timeValue = timer.getTime();
     
        // Update sceme:
        // Adjust camera:
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
            mork::mat3d mat = camera->getRotation();
            axis = mat*axis; // Transform axis to global
            camera->setRotation(mork::quatd(axis, radians(angle*this->getDt())).toMat4().mat3x3()*mat);
        }

        if(keys.count('W')) {
            // Move camera foward 2.0 units / s
            mork::vec3d pos = camera->getPosition().xyz();
            pos += camera->getWorldForward()*this->getDt()*2.0;
            camera->setPosition(mork::vec4d(pos));
        } else if(keys.count('S')) {
            // Move camera backwards 2.0 units / s
            mork::vec3d pos = camera->getPosition().xyz();
            pos -= camera->getWorldForward()*this->getDt()*2.0;
            camera->setPosition(mork::vec4d(pos));
        } else if(keys.count('A')) {
            // Move camera left 2.0 units / s
            mork::vec3d pos = camera->getPosition().xyz();
            pos -= camera->getWorldRight()*this->getDt()*2.0;
            camera->setPosition(mork::vec4d(pos));
        } else if(keys.count('D')) {
            // Move camera right 2.0 units / s
            mork::vec3d pos = camera->getPosition().xyz();
            pos += camera->getWorldRight()*this->getDt()*2.0;
            camera->setPosition(mork::vec4d(pos));
        }
 
        
        scene.update();
    
        // Prepare rendering
        mork::DirLight dirLight;
        dirLight.setDirection(mork::vec3d(1,-1,0));
        dirLight.setAmbientColor(mork::vec3d(0.2, 0.2, 0.2));


        mork::SpotLight spotLight;
        spotLight.setDirection(camera->getWorldForward());
        spotLight.setPosition(camera->getWorldPos());
        
        //spotLight.setAmbientColor(mork::vec3d(0.05, 0.05, 0.05));

        mork::PointLight pointLight;
      
        mork::mat4d view = camera->getViewMatrix();

        mork::mat4d proj = camera->getProjectionMatrix(); 
            

        // draw scene:
        auto& model = scene.getRoot().getChild("model");

        mork::mat4d modelMat = model.getLocalToWorld();
        mork::mat3d normalMat = ((modelMat.inverse()).transpose()).mat3x3();
        //mork::mat4f trans = (proj*view*model).cast<float>();
        prog.use();
        prog.getUniform("projection").set(proj.cast<float>());
        prog.getUniform("view").set(view.cast<float>());
        prog.getUniform("model").set(modelMat.cast<float>());
        prog.getUniform("normalMat").set(normalMat.cast<float>());
       
        prog.getUniform("viewPos").set(camera->getLocalToWorld().translation().cast<float>());

        dirLight.set(prog, "dirLight");
 
        spotLight.set(prog, "spotLight");
       
        pointLight.set(prog, "pointLight");
        

        model.draw(prog);

        // local function to draw recursive tree bounding boxes:
        struct local {
            void drawBox(const mork::SceneNode& node, const mork::mat4d& proj, const mork::mat4d& view) {
                mork::BBoxDrawer::drawBox(node.getWorldBounds(), proj, view);                
                for(const mork::SceneNode& child : node.getChildren()) {
                    drawBox(child, proj, view);
                }
                

            }
        } f; 

        f.drawBox(model, proj, view);

        // Draw 2D Text 
        glDisable(GL_DEPTH_TEST);
        
        mork::mat4f ortho = mork::mat4f::orthoProjection(this->getWidth(), 0.0f, this->getHeight(), 0.0f, -1.0f, 1.0f);

        std::stringstream info;
        info << "Multiline text:\n";
        info << "FPS: " << this->getFps() << ", frametime: " << this->getFrameTime() << "s\n";
        info << "Key menu:(entries are tabbed)\n";
        //info << "\tPress [0-9] to toggle focus object (0 = none)\n";
        info << "\tPress [ESC] to quit\n"; 
        info << "Keys: ";
        for(auto c: keys)
            info << c << "[" << (int)c << "], ";

        font.drawText(info.str(), 25, this->getHeight()-50, 18, mork::vec3f(1.0, 1.0, 1.0), ortho);

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
/*
        if(c >= 48 && c <= 57)
        {
            int obj = c-48;
    
            if(obj>0 && obj < 10)
                camera->setReference(nodes[obj]);
            else if(obj == 0)
                camera->setReference(nullptr);

            return true;

        }
*/
        return false;
    }

    virtual bool keyReleased(unsigned char c, modifier m, int x, int y) {
        keys.erase(c);

        return false;
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

    mork::Timer timer;

    mork::Font font;

    mork::Program prog;
    mork::Texture<2> diffuseMap, specularMap;

    mork::Material material;
    std::set<char> keys;
 
    mork::Scene scene;

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


