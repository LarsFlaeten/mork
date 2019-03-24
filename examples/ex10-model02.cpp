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
#include "mork/render/Framebuffer.h"
#include "mork/core/stb_image.h"


#include "mork/util/BBoxDrawer.h"

using namespace std;

static std::string window_title = "Model 02";

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aNorm;\n"
    "layout (location = 2) in vec3 aTang;\n"
    "layout (location = 3) in vec3 aBitang;\n"
    "layout (location = 4) in vec2 aUv;\n"
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
    "uniform samplerCube environment;\n"
    "\n"
    "float near = 0.1;\n" 
    "float far  = 100.0;\n" 
    "\n"
    "float linearizeDepth(float depth)\n"
    "{\n"
    "   float z = depth * 2.0 - 1.0; // back to NDC\n" 
    "   return (2.0 * near * far) / (far + near - z * (far - near));\n"
    "}\n"
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
    "   // Reflective color:\n"
    "   if(material.reflectiveFactor>0.0) {\n"
    "       vec3 I = normalize(fragPos - viewPos);\n"
    "       vec3 R = reflect(I, normalize(normal));\n"
    "       lightResult += material.reflectiveFactor*texture(environment, R).rgb;\n"
    "   }\n"
    "   // Refractive color:\n"
    "   if(material.refractiveFactor>0.0) {\n"
    "       float ratio = 1.0 / material.refractiveIndex;\n"
    "       vec3 I = normalize(fragPos - viewPos);\n"
    "       vec3 R = refract(I, normalize(normal), ratio);\n"
    "       lightResult += material.refractiveFactor*texture(environment, R).rgb;\n"
    "   }\n"
    "\n"
    "   vec3 total = emissiveColor + lightResult;\n"
    "\n"
    "   float depth = linearizeDepth(gl_FragCoord.z)/far;\n"
    "   FragColor = vec4(total, 1.0);\n"
    "}\n\0";

const char *vs_quad_source = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aNorm;\n"
    "layout (location = 2) in vec2 aUv;\n"
    "out vec2 texCoord;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
    "   texCoord = aUv;\n"
    "}\0";

const char *fs_quad_source = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 texCoord;\n"
    "\n"
    "uniform sampler2D tex;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   vec4 color = texture(tex, texCoord);\n"
    "   FragColor = vec4(color.rgba);\n"
    "}\n\0";

const char* vs_skybox = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aNorm;\n"
    "layout (location = 2) in vec2 aUv;\n"
    "\n"
    "out vec3 TexCoords;\n"
    "\n"
    "uniform mat4 projection;\n"
    "uniform mat4 view;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   TexCoords = aPos;\n"
    "   vec4 pos = projection * view * vec4(aPos, 1.0);\n"
    "   // Trick, set z = w, so that the z ndc coordinate will allways be 1.0\n"
    "   // (since ndc.z = z/w = w/w == 1.0\n"
    "   gl_Position = pos.xyww;\n"
    "}\n\0";

const char* fs_skybox = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "\n"
    "in vec3 TexCoords;\n"
    "\n"
    "uniform samplerCube skybox;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   FragColor = texture(skybox, TexCoords);\n"
    "}\n\0";

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

class App : public mork::GlfwWindow {
public:
    App()
            : mork::GlfwWindow(mork::Window::Parameters().size(800,600).name(window_title)),
               prog(std::string(vertexShaderSource), std::string(fragmentShaderSource)),
               font(mork::Font::createFont("resources/fonts/LiberationSans-Regular.ttf", 48)),
               textBox(800, 600),
               fsQuad(mork::MeshHelper<VN>::PLANE()),
               quadProg(std::string(vs_quad_source), std::string(fs_quad_source)),
               skyboxProg(std::string(vs_skybox), std::string(fs_skybox)),
               skybox(mork::MeshHelper<VN>::BOX()),
               environment()
    {
        up = false;
        down = false;
        left = false;
        right = false;

         
        auto teapot1 = std::make_unique<mork::Model>(mork::ModelImporter::loadModel("models/", "teapot.nff", "teapot1")); 
        teapot1->setLocalToParent(mork::mat4d::translate(mork::vec3d(0,5,0))*mork::mat4d::rotatex(radians(-90.0)));
        //teapot1->setLocalToParent(mork::mat4d::translate(mork::vec3d(0,5,0))*mork::mat4d::rotatez(radians(-90.0)));
        scene.getRoot().addChild(std::move(teapot1));

        auto teapot2 = std::make_unique<mork::Model>(mork::ModelImporter::loadModel("models/", "teapot.nff", "teapot2")); 
        teapot2->setLocalToParent(mork::mat4d::translate(mork::vec3d(0,-5,0))*mork::mat4d::rotatex(radians(-90.0)));
        //teapot2->setLocalToParent(mork::mat4d::translate(mork::vec3d(0,-5,0))*mork::mat4d::rotatez(radians(-90.0)));
        scene.getRoot().addChild(std::move(teapot2));


               
        scene.getCamera().setPosition(mork::vec4d(-10, 0, 0, 1));
        scene.getCamera().lookAt(mork::vec3d(1,0,0), mork::vec3d(0, 1, 0));
        scene.getCamera().setClippingPlanes(0.1,100);

        mork::GlfwWindow::waitForVSync(false);

        // Generate cubemap:
        vector<std::string> faces = 
        {
                "textures/skybox/right.jpg",
                "textures/skybox/left.jpg",
                "textures/skybox/top.jpg",
                "textures/skybox/bottom.jpg",
                "textures/skybox/front.jpg",
                "textures/skybox/back.jpg"
        };
        

        environment.loadTextures(faces);  

        // Modify teapot1's material to pure reflection:
        auto& model1 = dynamic_cast<mork::Model&>(scene.getRoot().getChild("teapot1"));
        auto& materials1 = model1.getMaterials();
        // Set pure reflective:
        materials1[0].reflectiveFactor = 0.0f;
        materials1[0].refractiveFactor = 1.0f;
        materials1[0].refractiveIndex = 1.52f;
        materials1[0].ambientColor = mork::vec3f::ZERO;
        materials1[0].diffuseColor = mork::vec3f::ZERO;
        materials1[0].specularColor = mork::vec3f::ZERO;
        materials1[0].emissiveColor = mork::vec3f::ZERO;

        // Modify teapot2's material to pure refraction:
        auto& model2 = dynamic_cast<mork::Model&>(scene.getRoot().getChild("teapot2"));
        auto& materials2 = model2.getMaterials();
        // Set pure reflective:
        materials2[0].reflectiveFactor = 1.0f;
        materials2[0].refractiveFactor = 0.0f;
        materials2[0].refractiveIndex = 1.52f;
        materials2[0].ambientColor = mork::vec3f::ZERO;
        materials2[0].diffuseColor = mork::vec3f::ZERO;
        materials2[0].specularColor = mork::vec3f::ZERO;
        materials2[0].emissiveColor = mork::vec3f::ZERO;





    }
/*	
	unsigned int loadCubemap(const std::vector<std::string>& faces)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
							 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
				);
				stbi_image_free(data);
			}
			else
			{
				std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		return textureID;
	}   
  */  

	~App() {
    }

    virtual void redisplay(double t, double dt) {
       
        // Framebuffer ops 
        mork::Framebuffer::getDefault().bind();
        mork::Framebuffer::getDefault().clear();

        // State group:
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
 
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
        dirLight.setDirection(mork::vec3d(1,-1,0));
        dirLight.setAmbientColor(mork::vec3d(0.2, 0.2, 0.2));

        
        // Prepare for normal rendering:
        glDepthMask(GL_TRUE);
 
        mork::SpotLight spotLight;
        spotLight.setDirection(camera.getWorldForward());
        spotLight.setPosition(camera.getWorldPos());
        
        mork::PointLight pointLight;
      
           
        prog.use();
        dirLight.set(prog, "dirLight");
 
        spotLight.set(prog, "spotLight");
       
        pointLight.set(prog, "pointLight");
       
        // Allways use slot 16 as the enviroment texture, as 0-15 may be used by Material 
        prog.getUniform("environment").set(16);
        environment.bind(16);
 
        scene.draw(prog);
         
        
        // Draw skybox:
        // Need to change to less than or equal to max the skybox pass the depth test 
        // (z == 1, see the vs shader)
        glDepthFunc(GL_LEQUAL);
        skyboxProg.use();
        // Take only the rotational part of the view matrix so
        // that we are allways in the center of the skybox
        mork::mat4d viewCenter = mork::mat4d(camera.getViewMatrix().mat3x3()); 
        // Rotate the skybox 90 degrees by x:
        // TOOD: FInd out why we need to do this....
        //viewCenter = viewCenter*mork::mat4d::rotatex(radians(90.0));


      
        skyboxProg.getUniform("view").set(viewCenter.cast<float>());
        skyboxProg.getUniform("projection").set(camera.getProjectionMatrix().cast<float>());
        
        environment.bind();
        skybox.draw();
        environment.unbind();

       
        // Draw 2D Text 
        
        // State group, only diff is shown
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        
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
                

        textBox.setText(info.str());
        textBox.drawToBuffer(font);

        // State changes
        glEnable(GL_BLEND); 
        mork::Framebuffer::getDefault().bind();
        
        quadProg.use(); 
        quadProg.getUniform("tex").set(0);
        textBox.getColorBuffer().bind(0);
        fsQuad.draw();

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
    mork::Program quadProg;
    mork::Program skyboxProg;
    std::set<char> keys;
 
    mork::Scene scene;

    TextBox textBox;

    mork::Mesh<VN>  fsQuad;
    mork::Mesh<VN>  skybox;

    mork::CubeMapTexture environment;
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


