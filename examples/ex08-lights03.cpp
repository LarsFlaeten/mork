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
#include "mork/render/Light.h"
#include "mork/math/quat.h"
#include "mork/render/Mesh.h"
#include "mork/render/Material.h"

using namespace std;

static std::string window_title = "Lights 03";

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
               lampProg(std::string(vertexShaderSource), std::string(fragmentShaderSource2)),
              font(mork::Font::createFont("resources/fonts/LiberationSans-Regular.ttf", 48))
  {
      up = false;
      down = false;
      left = false;
      right = false;

      lampPos = mork::vec3d(-10.0/sqrt(2), -10.0/sqrt(2), 0.0);
        
        std::vector<mork::vertex_pos_norm_uv> verts = {
            {mork::vec3f(-1.0f, -1.0f, -1.0f), mork::vec3f(0,0,-1), mork::vec2f(0.0f, 0.0f)},
            {mork::vec3f( 1.0f, -1.0f, -1.0f), mork::vec3f(0,0,-1), mork::vec2f(1.0f, 0.0f)},
            {mork::vec3f( 1.0f,  1.0f, -1.0f), mork::vec3f(0,0,-1), mork::vec2f(1.0f, 1.0f)},
            {mork::vec3f( 1.0f,  1.0f, -1.0f), mork::vec3f(0,0,-1), mork::vec2f(1.0f, 1.0f)},
            {mork::vec3f(-1.0f,  1.0f, -1.0f), mork::vec3f(0,0,-1), mork::vec2f(0.0f, 1.0f)},
            {mork::vec3f(-1.0f, -1.0f, -1.0f), mork::vec3f(0,0,-1), mork::vec2f(0.0f, 0.0f)},

            {mork::vec3f(-1.0f, -1.0f,  1.0f), mork::vec3f(0,0,1),  mork::vec2f(0.0f, 0.0f)},
            {mork::vec3f( 1.0f, -1.0f,  1.0f), mork::vec3f(0,0,1),  mork::vec2f(1.0f, 0.0f)},
            {mork::vec3f( 1.0f,  1.0f,  1.0f), mork::vec3f(0,0,1),  mork::vec2f(1.0f, 1.0f)},
            {mork::vec3f( 1.0f,  1.0f,  1.0f), mork::vec3f(0,0,1),  mork::vec2f(1.0f, 1.0f)},
            {mork::vec3f(-1.0f,  1.0f,  1.0f), mork::vec3f(0,0,1),  mork::vec2f(0.0f, 1.0f)},
            {mork::vec3f(-1.0f, -1.0f,  1.0f), mork::vec3f(0,0,1),  mork::vec2f(0.0f, 0.0f)},

            {mork::vec3f(-1.0f,  1.0f,  1.0f), mork::vec3f(-1,0,0),  mork::vec2f(1.0f, 0.0f)},
            {mork::vec3f(-1.0f,  1.0f, -1.0f), mork::vec3f(-1,0,0),  mork::vec2f(1.0f, 1.0f)},
            {mork::vec3f(-1.0f, -1.0f, -1.0f), mork::vec3f(-1,0,0),  mork::vec2f(0.0f, 1.0f)},
            {mork::vec3f(-1.0f, -1.0f, -1.0f), mork::vec3f(-1,0,0),  mork::vec2f(0.0f, 1.0f)},
            {mork::vec3f(-1.0f, -1.0f,  1.0f), mork::vec3f(-1,0,0),  mork::vec2f(0.0f, 0.0f)},
            {mork::vec3f(-1.0f,  1.0f,  1.0f), mork::vec3f(-1,0,0),  mork::vec2f(1.0f, 0.0f)},

            {mork::vec3f(1.0f,  1.0f,  1.0f), mork::vec3f(1,0,0),  mork::vec2f(1.0f, 0.0f)},
            {mork::vec3f( 1.0f,  1.0f, -1.0f), mork::vec3f(1,0,0),  mork::vec2f(1.0f, 1.0f)},
            {mork::vec3f(  1.0f, -1.0f, -1.0f), mork::vec3f(1,0,0),  mork::vec2f(0.0f, 1.0f)},
            {mork::vec3f( 1.0f, -1.0f, -1.0f), mork::vec3f(1,0,0),  mork::vec2f(0.0f, 1.0f)},
            {mork::vec3f( 1.0f, -1.0f,  1.0f), mork::vec3f(1,0,0),  mork::vec2f(0.0f, 0.0f)},
            {mork::vec3f( 1.0f,  1.0f,  1.0f), mork::vec3f(1,0,0),  mork::vec2f(1.0f, 0.0f)},

            {mork::vec3f(-1.0f, -1.0f, -1.0f), mork::vec3f(0,-1,0),  mork::vec2f(0.0f, 1.0f)},
            {mork::vec3f( 1.0f, -1.0f, -1.0f), mork::vec3f(0,-1,0),  mork::vec2f(1.0f, 1.0f)},
            {mork::vec3f( 1.0f, -1.0f,  1.0f), mork::vec3f(0,-1,0),  mork::vec2f(1.0f, 0.0f)},
            {mork::vec3f( 1.0f, -1.0f,  1.0f), mork::vec3f(0,-1,0),  mork::vec2f(1.0f, 0.0f)},
            {mork::vec3f(-1.0f, -1.0f,  1.0f), mork::vec3f(0,-1,0),  mork::vec2f(0.0f, 0.0f)},
            {mork::vec3f(-1.0f, -1.0f, -1.0f), mork::vec3f(0,-1,0),  mork::vec2f(0.0f, 1.0f)},

            {mork::vec3f(-1.0f,  1.0f, -1.0f), mork::vec3f(0,1,0),  mork::vec2f(0.0f, 1.0f)},
            {mork::vec3f( 1.0f,  1.0f, -1.0f), mork::vec3f(0,1,0),  mork::vec2f(1.0f, 1.0f)},
            {mork::vec3f( 1.0f,  1.0f,  1.0f), mork::vec3f(0,1,0),  mork::vec2f(1.0f, 0.0f)},
            {mork::vec3f( 1.0f,  1.0f,  1.0f), mork::vec3f(0,1,0),  mork::vec2f(1.0f, 0.0f)},
            {mork::vec3f(-1.0f,  1.0f,  1.0f), mork::vec3f(0,1,0),  mork::vec2f(0.0f, 0.0f)},
            {mork::vec3f(-1.0f,  1.0f, -1.0f), mork::vec3f(0,1,0),  mork::vec2f(0.0f, 1.0f)}
        };       
        
       
        vao.bind();
        vb.bind();
        vb.setData(verts);
        vb.setAttributes();
        vb.unbind();
        vao.unbind();

        positions = {
            mork::vec3d(0, 0, 0),
            mork::vec3d(0, 5, 4),
            mork::vec3d(0, 0, 3),
            mork::vec3d(0, 3, 0),
            mork::vec3d(0, -3, -1),
            mork::vec3d(-20, 0, 0),
            mork::vec3d(0, 10, 4),
            mork::vec3d(-2, -11, 4),
            mork::vec3d(-2, 1, 14),
            mork::vec3d(-2, 1, -14),
        };


        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        mork::Texture<2> diffuse1;
        diffuse1.loadTexture("textures/container2.png", true);
        mork::TextureLayer tl_diff1(std::move(diffuse1), mork::Op::ADD, 0.8f);

        mork::Texture<2> diffuse2;
        diffuse2.loadTexture("textures/awesomeface.png", true);
        mork::TextureLayer tl_diff2(std::move(diffuse2), mork::Op::ADD, 0.2f);


        mork::Texture<2> specular;
        specular.loadTexture("textures/container2_specular.png", true);
        mork::TextureLayer tl_spec(std::move(specular));

        material.diffuseLayers.push_back(std::move(tl_diff1));
        material.diffuseLayers.push_back(std::move(tl_diff2));
        material.specularLayers.push_back(std::move(tl_spec));


        for(int i = 0; i < 10; ++i) {
            auto& box = scene.getRoot().addChild(mork::SceneNode(std::string("box") + std::to_string(i+1)));
            box.setLocalBounds(mork::box3d(-1, 1, -1, 1, -1, 1));
        }

        scene.getRoot().addChild(mork::SceneNode("lamp"));

        scene.getCamera().setPosition(mork::vec3d(-12, 0, 0));
        scene.getCamera().lookAt(mork::vec3d(1,0,0), mork::vec3d(0, 0, 1));

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 
        waitForVSync(false);
    }

    ~App() {
    }

    virtual void redisplay(double t, double dt) {


        glEnable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    

        double timeValue = timer.getTime();
     
        // Update sceme:
        int i = 1;
        for(mork::SceneNode& box : scene.getRoot().getChildren()) {
            box.setLocalToParent(
                mork::mat4d::translate(positions[i++])*mork::mat4d::rotatez(0.01*timeValue)*mork::mat4d::rotatey(0.1*timeValue));
        }


        auto& lamp = scene.getRoot().getChild("lamp");
        lamp.setLocalToParent(mork::mat4d::scale(mork::vec3d(0.5, 0.5, 0.5))*mork::mat4d::translate(lampPos));


        auto& camera = scene.getCamera();       
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
            mork::mat3d mat = camera.getRotation();
            axis = mat*axis; // Transform axis to global
            camera.setRotation(mork::quatd(axis, radians(angle*this->getDt())).toMat4().mat3x3()*mat);
        }
        
        scene.update();
    
        // Prepare rendering

        std::stringstream box_viz, box_pviz, box_iviz;
       
        mork::mat4d view = camera.getViewMatrix();

        mork::mat4d proj = camera.getProjectionMatrix(); 
            
        mork::PointLight pointLight;
        pointLight.setPosition( lamp.getLocalToWorld().translation()); 
        pointLight.setAmbientColor(mork::vec3d::ZERO);

        mork::DirLight dirLight;
        dirLight.setDirection(mork::vec3d(0,-1,0));
        //dirLight.setDiffuseColor(mork::vec3d(1,0, 0));
        dirLight.setAmbientColor(mork::vec3d::ZERO);


        mork::SpotLight spotLight;
        spotLight.setDirection(camera.getWorldForward());
        spotLight.setPosition(camera.getWorldPos());
        spotLight.setAmbientColor(mork::vec3d::ZERO);

       // draw boxes:
        int boxcount = 0;
                 
        prog.use();
        
        pointLight.set(prog, "pointLight");
                

        dirLight.set(prog, "dirLight");
         
        spotLight.set(prog, "spotLight");
                
        material.set(prog, "material");
        material.bindTextures();
        
        
        for(int i = 0; i < 10; ++i) {
            std::string name = std::string("box") + std::to_string(i+1);
            auto& box = scene.getRoot().getChild(name);

            mork::Visibility viz = camera.getWorldFrustum().getVisibility(box.getWorldBounds());
            ++boxcount;
            if(viz != mork::Visibility::INVISIBLE) {

                if(viz == mork::Visibility::PARTIALLY_VISIBLE) {
                    box_pviz << box.getName() << "\n";
                    pointLight.setDiffuseColor(mork::vec3d(1,0,0));
                    dirLight.setDiffuseColor(mork::vec3d(1,0,0));
                    spotLight.setDiffuseColor(mork::vec3d(1,0,0));
                } else {
                    box_viz << box.getName() << "\n";
                    pointLight.setDiffuseColor(mork::vec3d(1,1,1));
                    dirLight.setDiffuseColor(mork::vec3d(1,1,1));
                    spotLight.setDiffuseColor(mork::vec3d(1,1,1));
 
                }

                mork::mat4d model = box.getLocalToWorld();
                mork::mat3d normalMat = ((model.inverse()).transpose()).mat3x3();
                //mork::mat4f trans = (proj*view*model).cast<float>();
                prog.getUniform("projection").set(proj.cast<float>());
                prog.getUniform("view").set(view.cast<float>());
                prog.getUniform("model").set(model.cast<float>());
                prog.getUniform("normalMat").set(normalMat.cast<float>());
               
                prog.getUniform("viewPos").set(camera.getLocalToWorld().translation().cast<float>());
     
              
                vao.bind();
                glDrawArrays(GL_TRIANGLES, 0, 36); 
                
            } else {
                box_iviz << box.getName() << "\n";
            }
        }

        // draw lamp:
        mork::mat4d model2 = lamp.getLocalToWorld();
        lampProg.use();
        lampProg.getUniform("projection").set(proj.cast<float>());
        lampProg.getUniform("view").set(view.cast<float>());
        lampProg.getUniform("model").set(model2.cast<float>());
        lampProg.getUniform("color").set(mork::vec3f(1.0, 1.0, 1.0));
        vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, 36); 

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
            info << c.first << "[" << (int)c.first << "], ";

        font.drawText(info.str(), 25, this->getHeight()-50, 18, mork::vec3f(1.0, 1.0, 1.0), ortho);
        std::stringstream info2;
        info2 << "Fully Visible boxes:\n" << box_viz.str();
        info2 << "Partly Visible boxes:\n" << box_pviz.str();
        info2 << "InVisible boxes:\n" << box_iviz.str();
       
        font.drawText(info2.str(), this->getWidth()-200, this->getHeight()-50, 18, mork::vec3f(1,1,1), ortho);




        GlfwWindow::redisplay(t, dt);
  
    }

    virtual void reshape(int x, int y)
    {
        // TODO:
        // Move to new Framebuffer class
        glViewport(0, 0, x, y);
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
        keys[c] = true;
/*
        if(c >= 48 && c <= 57)
        {
            int obj = c-48;
    
            if(obj>0 && obj < 10)
                camera.setReference(nodes[obj]);
            else if(obj == 0)
                camera.setReference(nullptr);

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
       
        float ra = static_cast<float>(x)/static_cast<float>(this->getWidth());
        float dec = static_cast<float>(y)/static_cast<float>(this->getHeight());
        ra = radians(180.0f*(-ra*2.0 + 1.0));
        dec = radians(90.0f*(-dec*2.0 + 1.0));
        lampPos = mork::vec3d(-10.0*cos(ra)*cos(dec), 10.0*sin(ra)*cos(dec), 10.0*sin(dec));


        return true;
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

    std::map<char, bool> keys;

    mork::VertexBuffer<mork::vertex_pos_norm_uv> vb;
    mork::GPUBuffer<unsigned int, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW> ib;

    //mork::Texture<2> diffuseMap, specularMap;
    mork::Material material;

    mork::VertexArrayObject vao;
    mork::Program prog, lampProg;    
 
    mork::Scene scene;

    std::vector< mork::vec3d > positions;

    mork::vec3d lampPos;
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


