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
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec4 ourColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = ourColor;\n"
    "}\n\0";

const char *vertexShaderSource2 = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec4 aCol;\n"
    "out vec4 color;"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "   color = aCol;\n"
    "}\0";

const char *fragmentShaderSource2 = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec4 color;\n"
    "void main()\n"
    "{\n"
    "   FragColor = color;\n"
    "}\n\0";

const char *vertexShaderSource3 = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec4 aCol;\n"
    "layout (location = 2) in vec2 aUV;\n"
    "out vec4 color;\n"
    "out vec2 texCoord;\n" 
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "   color = aCol;\n"
    "   texCoord = aUV;\n"
    "}\0";

const char *fragmentShaderSource3 = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec4 color;\n"
    "in vec2 texCoord;\n"
    "\n"
    "uniform sampler2D ourTexture;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   FragColor = color*texture(ourTexture, texCoord);\n"
    "}\n\0";

const char *vertexShaderSource4 = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aUV;\n"
    "out vec2 texCoord;\n" 
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "   texCoord = aUV;\n"
    "}\0";

const char *fragmentShaderSource4 = "#version 330 core\n"
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
            prog(std::string(vertexShaderSource), std::string(fragmentShaderSource)),
            prog2(std::string(vertexShaderSource2), std::string(fragmentShaderSource2)),
            prog3(std::string(vertexShaderSource3), std::string(fragmentShaderSource3)),
            prog4(std::string(vertexShaderSource4), std::string(fragmentShaderSource4))
        


    {
       // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------
        //std::vectpr<int>
        std::vector<mork::vertex_pos> vertices = {
            mork::vec3f(0.0f,  0.5f, 0.0f),  // top right
            mork::vec3f(0.0f, -0.5f, 0.0f),  // bottom right
            mork::vec3f(-0.75f, -0.5f, 0.0f),  // bottom left
            mork::vec3f(-0.75f,  0.5f, 0.0f)   // top left 
        };
        std::vector<unsigned int> indices = {  
            0, 1, 3,  // first Triangle
            1, 2, 3   // second Triangle
        };

        std::vector<mork::vertex_pos_col> verts2 = {
            { mork::vec3f(-0.5f, 0.0f, 0.0f), mork::vec4f(1,0,0,1) },
            { mork::vec3f(-1.0f, 0.0f, 0.0f), mork::vec4f(0,1,0,1) },
            { mork::vec3f(-0.75f, 1.0f, 0.0f), mork::vec4f(0,0,1,1) },
        };
        
        std::vector<unsigned int> indices2 = {  
            0, 1, 2  // first Triangle
        };

        std::vector<mork::vertex_pos_col_uv> verts3 = {
    		// positions          // colors           // texture coords
            {mork::vec3f(1.0f,  0.5f, 0.0f),   mork::vec4f(1.0f, 0.0f, 0.0f, 1.0f),   mork::vec2f(1.0f, 1.0f)},   // top right
            {mork::vec3f(1.0f, -0.4f, 0.0f),   mork::vec4f(0.0f, 1.0f, 0.0f, 1.0f),   mork::vec2f(1.0f, 0.0f)},   // bottom right
            {mork::vec3f(0.1f, -0.4f, 0.0f),   mork::vec4f(0.0f, 0.0f, 1.0f, 1.0f),   mork::vec2f(0.0f, 0.0f)},   // bottom left
            {mork::vec3f(0.1f,  0.5f, 0.0f),   mork::vec4f(1.0f, 1.0f, 0.0f, 1.0f),   mork::vec2f(0.0f, 1.0f)}    // top left 
		}; 
        
        std::vector<mork::vertex_pos_uv> verts4 = {
    		// positions                    // texture coords
            {mork::vec3f(1.0f,  1.0f, 0.0f),   mork::vec2f(1.0f, 1.0f)},   // top right
            {mork::vec3f(1.0f,  0.55f, 0.0f),   mork::vec2f(1.0f, 0.0f)},   // bottom right
            {mork::vec3f(0.1f,  0.55f, 0.0f),   mork::vec2f(0.0f, 0.0f)},   // bottom left
            {mork::vec3f(0.1f,  1.0f, 0.0f),   mork::vec2f(0.0f, 1.0f)}    // top left 
		}; 
       // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        vao.bind();

        // This will bind and set vb data:
        vb.bind();
        vb.setData(vertices);
        vb.setAttributes();
         
        ib.bind();
        ib.setData(indices);
        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        vb.unbind(); 

        // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        vao.unbind();

        vao2.bind();
        vb2.bind();
        vb2.setData(verts2);
        vb2.setAttributes();

        ib2.bind();
        ib2.setData(indices2);
        vb2.unbind();
        vao2.unbind();

        vao3.bind();
        vb3.bind();
        vb3.setData(verts3);
        vb3.setAttributes();
        ib3.bind();
        ib3.setData(indices);
        vb3.unbind();
        vao3.unbind();

        vao4.bind();
        vb4.bind();
        vb4.setData(verts4);
        vb4.setAttributes();
        ib4.bind();
        ib4.setData(indices);
        vb4.unbind();
        vao4.unbind();


        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        tex1.loadTexture("textures/container.jpg", false);
        tex2.loadTexture("textures/awesomeface.png", true);
        prog4.use();
        prog4.getUniform("texture1").set(0);
        prog4.getUniform("texture2").set(1);


    }

    ~App() {
    }

    virtual void redisplay(double t, double dt) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);    

         // draw our first triangle
        double timeValue = timer.getTime();
        double greenValue = (sin(timeValue) / 2.0) + 0.5;

        //mork::StaticUniform<mork::vec4f> color(prog, "ourColor");

        //int vertexColorLocation = glGetUniformLocation(prog.getProgramID(), "ourColor");
        
        prog.use();
     

        mork::vec4f col = mork::vec4f(0.0f, static_cast<float>(greenValue), 0.0f, 1.0f);
        prog.getUniform("ourColor").set(col);

        vao.bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        prog2.use();
        vao2.bind();
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        prog3.use();
        prog3.getUniform("ourTexture").set(0);
        vao3.bind();
        tex1.bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); 
        
        prog4.use();
        vao4.bind();
        glActiveTexture(GL_TEXTURE0 + 0);
        tex1.bind();
        glActiveTexture(GL_TEXTURE0 + 1);
        tex2.bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); 


   
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
    
    mork::VertexBuffer<mork::vertex_pos> vb;
    mork::VertexBuffer<mork::vertex_pos_col> vb2;
    mork::VertexBuffer<mork::vertex_pos_col_uv> vb3;
    mork::VertexBuffer<mork::vertex_pos_uv> vb4;
    mork::GPUBuffer<unsigned int, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW> ib;
    mork::GPUBuffer<unsigned int, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW> ib2;
    mork::GPUBuffer<unsigned int, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW> ib3;
    mork::GPUBuffer<unsigned int, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW> ib4;

    mork::Texture<2> tex1, tex2;


    mork::VertexArrayObject vao, vao2, vao3, vao4;
    mork::Program prog, prog2, prog3, prog4;    
         
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
        //("v,verbose", "Verbose debug mode", cxxopts::value<int>(verbose)->default_value("0")->implicit_value("1"))
        //("d,debug", "Debug mode - Warning - excessive logging!")
        //("s, server_adress", "Adress and port of server to conect to")
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
#if 0



    while(!glfwWindowShouldClose(window))
    {
        processInput(window);
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);    

        // draw our first triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        //glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind it every time 

        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

        
    // optional: de-allocate all resources once they've outlived their purpose:
    ./ ------------------------------------------------------------------------
  
    
    glfwTerminate();                             
#endif

    return 0;
}



