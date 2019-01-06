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

static std::string window_title = "Lights 02";

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
    "out vec4 FragColor;\n"
    "in vec2 texCoord;\n"
    "in vec3 fragPos;\n"
    "in vec3 normal;\n"
    "\n"
    "struct PositionalLight {\n"
    "   vec3 position;\n"
    "   vec3 ambient;\n"
    "   vec3 diffuse;\n"
    "   vec3 specular;\n"
    "};\n"
    "\n"
    "struct TexturedMaterial {\n"
    "   sampler2D diffuseMap;\n"
    "   sampler2D specularMap;\n"
    "   float shininess;\n"
    "};\n"
    "\n"
    "uniform PositionalLight light;\n"
    "uniform TexturedMaterial material;\n"
    "uniform vec3 viewPos;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   vec3 ambient = light.ambient * texture(material.diffuseMap, texCoord).xyz;\n"
    "\n" 
    "   vec3 norm = normalize(normal);\n"
    "   vec3 lightDir = normalize(light.position - fragPos);\n"
    "   float diff = max(dot(norm, lightDir), 0.0);\n"
    "   vec3 diffuse = light.diffuse * diff * texture( material.diffuseMap, texCoord ).xyz;\n"
    "\n"
    "   vec3 viewDir = normalize(viewPos - fragPos);\n"
    "   vec3 reflectDir = reflect(-lightDir, norm);\n"
    "   float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);\n"
    "   vec3 specular = light.specular * spec * texture( material.specularMap, texCoord ).xyz;\n"
    "\n"
    "   FragColor = vec4((ambient + diffuse + specular), 1.0);\n"
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


        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        diffuseMap.loadTexture("textures/container2.png", true);
        specularMap.loadTexture("textures/container2_specular.png", true);
        prog.use();
        prog.getUniform("material.diffuseMap").set(0);
        prog.getUniform("material.specularMap").set(1);

        scene.getRoot().addChild(mork::SceneNode("box"));

        scene.getRoot().addChild(mork::SceneNode("lamp"));

        scene.getCamera().setFOV(radians(45.0));
        scene.getCamera().setPosition(mork::vec4d(-12, 0, 0, 1));
        scene.getCamera().lookAt(mork::vec3d(1,0,0), mork::vec3d(0, 0, 1));


        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 
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
        auto& box = scene.getRoot().getChild("box");
        auto& lamp = scene.getRoot().getChild("lamp");
        
        box.setLocalToParent(
                mork::mat4d::translate(mork::vec3d(0, 0, 0))*mork::mat4d::rotatez(0.01*timeValue)*mork::mat4d::rotatey(0.1*timeValue));
        
        
        

        lamp.setLocalToParent(mork::mat4d::scale(mork::vec3d(0.5, 0.5, 0.5))*mork::mat4d::translate(lampPos));
 
        
                
        scene.update();


        mork::Camera& camera = scene.getCamera(); 
        mork::mat4d view = camera.getViewMatrix();

        mork::mat4d proj = camera.getProjectionMatrix(); 

        // draw box:
        mork::mat4d model = box.getLocalToWorld();
        mork::mat3d normalMat = ((model.inverse()).transpose()).mat3x3();
        //mork::mat4f trans = (proj*view*model).cast<float>();
        prog.use();
        prog.getUniform("projection").set(proj.cast<float>());
        prog.getUniform("view").set(view.cast<float>());
        prog.getUniform("model").set(model.cast<float>());
        prog.getUniform("normalMat").set(normalMat.cast<float>());
       
        prog.getUniform("material.shininess").set(32.0f);

        prog.getUniform("light.ambient").set(mork::vec3f(0.2, 0.2, 0.2));
        prog.getUniform("light.diffuse").set(mork::vec3f(0.5, 0.5, 0.5));
        prog.getUniform("light.specular").set(mork::vec3f(1.0, 1.0, 1.0));
        prog.getUniform("light.position").set(lamp.getLocalToWorld().translation().cast<float>());
        
        prog.getUniform("viewPos").set(camera.getLocalToWorld().translation().cast<float>());
        vao.bind();
        diffuseMap.bind(0);
        specularMap.bind(1);
        glDrawArrays(GL_TRIANGLES, 0, 36); 


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



        font.drawText("Text", 25.0f, 25.0f, 18, mork::vec3f(1.0, 1.0, 1.0), ortho);



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
    mork::Timer timer;

    mork::Font font;

    std::map<char, bool> keys;

    mork::VertexBuffer<mork::vertex_pos_norm_uv> vb;
    mork::GPUBuffer<unsigned int, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW> ib;

    mork::Texture<2> diffuseMap, specularMap;


    mork::VertexArrayObject vao;
    mork::Program prog, lampProg;    
 
    mork::Scene scene;

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


