#include "mork/ui/GlfwWindow.h"
#include "mork/core/Log.h"
#include "mork/core/DebugMessageCallback.h"
#include "mork/render/Framebuffer.h"

#include <assert.h>
#include <stdexcept>


using namespace std;


namespace mork
{

// Utility function to check wether there is an active context
bool GlfwWindow::isContextActive() {
    bool isActive = true;
    try {
        GLFWwindow* w = glfwGetCurrentContext();
    } catch (std::runtime_error) {
        isActive = false;
    }

    return isActive;
}




GlfwWindow::GlfwWindow(const Parameters &params) : Window(params), glfwWindowHandle(NULL)
{
    // Implemented glfw user pointers:
    // http://stackoverflow.com/questions/7676971/pointing-to-a-function-that-is-a-class-member-glfw-setkeycallback

    // We start with allocating the error callback:
    glfwSetErrorCallback(errorCallback);

    // Initialise GLFW
    if( !glfwInit() )
    {
        error_logger("UI: Could not init GLFW!");
        throw std::runtime_error("");
        return;
    }
    glfwWindowHint(GLFW_SAMPLES, 4);
    vec2i ver = params.getVersion();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, ver.x);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, ver.y);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifndef NDEBUG
    info_logger("Creating debug context");
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#else
    info_logger("Not creating debug context");
#endif

    GLFWwindow* gwd = glfwCreateWindow(params.width(), params.height(), params.name().c_str(), NULL, NULL);
    glfwWindowHandle = (void*)gwd;

    if(glfwWindowHandle == NULL)
    {
        error_logger("UI: Could not create GLFW window!");
        glfwTerminate();
        throw std::runtime_error("");
        return;
    }


    damaged = false;
    t = 0.0;
    dt = 0.0;


    fps = 0;
    frameCount = 0;
    /*
     * TODO: Add possibility to go fullscreen
     */     

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(gwd, GLFW_STICKY_KEYS, GL_TRUE);
           


    // Set up callbacks
    glfwSetWindowSizeCallback(gwd, reshapeFunc);
    glfwSetWindowFocusCallback(gwd, focusFunc);
    glfwSetCursorPosCallback(gwd, mouseMotionFunc);
    glfwSetScrollCallback(gwd, scrollFunc);
    glfwSetKeyCallback(gwd, keyCallback);
    glfwSetMouseButtonCallback(gwd, mouseClickFunc);

    // The last thing is to assign this as the user pointer:
    glfwSetWindowUserPointer(gwd, (void*)this);


    glfwMakeContextCurrent(gwd);


    // Here we get the actual size we got from glfw
    // May not be the same as given as "hint"?
    int width, height;
    glfwGetFramebufferSize(gwd, &width, &height);
   
    // We should also call reshape function, as GLUT does,
    // since some of the examples sets a few states based on this,
    // however since we are in CTOR now, the derived function will not
    // be called. Examples that rely on setting states in reshape
    // must provide an initial value (See e.g. minimalglfw)..
    reshape(width, height);


    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        error_logger("UI: Failed to initialize GLAD");
        throw std::runtime_error("");
    }    

    GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
    	debug_logger("Registering OpenGL debug callbak..");
		glEnable(GL_DEBUG_OUTPUT);
    	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
    	glDebugMessageCallback(debugMessageCallback, nullptr);
    	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
    
    // do immeadiate swap
    //this->waitForVSync(false);
    
    // Proforma binding of default frambuffer.
    auto& fb = Framebuffer::getDefault();
    fb.bind(); 
    
    
    timer.start();
    fps_t0 = timer.end(); 
}

GlfwWindow::~GlfwWindow()
{
    glfwTerminate();
}    



int GlfwWindow::getWidth() const
{
    return size.x;
}

int GlfwWindow::getHeight() const
{
    return size.y;
}

void GlfwWindow::getMousePosition(int* x, int* y)
{
    GLFWwindow* gwd = (GLFWwindow*)glfwWindowHandle;
    double xpos, ypos;
    glfwGetCursorPos(gwd, &xpos, &ypos);
    *x = static_cast<int>(xpos);
    *y = static_cast<int>(ypos);
}

void GlfwWindow::shouldClose()
{
    GLFWwindow* gwd = (GLFWwindow*)glfwWindowHandle;
    glfwSetWindowShouldClose(gwd, true);
}

void GlfwWindow::start()
{
    // Do the reshape call (will call derived), as a lot of the ork/proland
    // apps are setting variables based on this:
    this->reshape(this->getWidth(), this->getHeight());
    
    do{
           
        this->redisplay(t, dt);

        glfwPollEvents();
        
        
        
    } while(glfwWindowShouldClose((GLFWwindow*)glfwWindowHandle)==0);

    glfwTerminate();
}

void GlfwWindow::redisplay(double t, double dt)
{
    glfwSwapBuffers((GLFWwindow*)glfwWindowHandle);
    double newT = timer.end();
    this->dt = newT - this->t;
    this->t = newT;

    ++frameCount;
    double fps_dt = newT - fps_t0;
    if(fps_dt >= 1.0) {
        fps_t0 += fps_dt;
        fps = frameCount/fps_dt;
        frameCount = 0;
        fpsUpdatedEvent();
    }
}

void GlfwWindow::reshape(int x, int y)
{
    size = vec2i(x, y);
}

void GlfwWindow::idle(bool damaged)
{
    //glutPostRedisplay();
}

void GlfwWindow::redisplayFunc()
{
    throw std::runtime_error("Not implemented");
    // TODO: modify for multiple windows
    //GlfwWindow* window = INSTANCES[1];
    ////window->redisplay(window->t, window->dt);
}

double GlfwWindow::getFps() const {
    return fps;
}

double GlfwWindow::getFrameTime() const {
    if(fps>0.0)
        return 1.0/fps;
    else
        return 0.0;
}

double GlfwWindow::getDt() const {
    return dt;
}

// Override to catch fps update events..
void GlfwWindow::fpsUpdatedEvent() {

}


void GlfwWindow::reshapeFunc(GLFWwindow* window, int w, int h)
{
    GlfwWindow* gw = static_cast<GlfwWindow*>(glfwGetWindowUserPointer(window));
    gw->reshape(w, h);
}

/*void GlfwWindow::idleFunc()
{
    
    GlfwWindow* gw = static_cast<GlfwWindow*>(glfwGetWindowUserPointer(window));
    // glutLayerGet(GLUT_NORMAL_DAMAGED) is not implemented in freeglut
    //window->idle(glutLayerGet(GLUT_NORMAL_DAMAGED) == 1);
    gw->idle(window->damaged);
    gw->damaged = false;
*/
void GlfwWindow::mouseClickFunc(GLFWwindow* window, int mousebutton, int action, int mods)
{
    // Translate to Ork mouse messages
    EventHandler::button b;
    switch(mousebutton) {
        case GLFW_MOUSE_BUTTON_RIGHT:
            b = EventHandler::RIGHT_BUTTON;
            break;
        case GLFW_MOUSE_BUTTON_LEFT:
            b = EventHandler::LEFT_BUTTON;
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            b = EventHandler::MIDDLE_BUTTON;
            break;
        default:
            break;
    }
    
    EventHandler::state s;
    if(action==GLFW_PRESS)
        s = EventHandler::DOWN;
    else if(action==GLFW_RELEASE)
        s = EventHandler::UP;

    // Check for modifier keys:
    EventHandler::modifier m = getModifiers(window);

    // Get the mous cordinates:
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);


    // Call the class method:
    GlfwWindow* gw = static_cast<GlfwWindow*>(glfwGetWindowUserPointer(window));
    gw->mouseClick(b, s, m, static_cast<int>(xpos),static_cast<int>(ypos));


}

void GlfwWindow::scrollFunc(GLFWwindow* window, double scrollx,double scrolly)
{
    // Get modifiers:
    EventHandler::modifier m = getModifiers(window);

    // Mouse wheel provides y-scroll:
    // We discard x-scroll for now...
    EventHandler::wheel w;
    if(scrolly != 0.0) {

        if(scrolly < 0.0)
            w = WHEEL_DOWN; // Correct orientation?
        if(scrolly > 0.0)
            w = WHEEL_UP;

        // Get the mous cordinates:
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
          
          
        // Call the class method:
        GlfwWindow* gw = static_cast<GlfwWindow*>(glfwGetWindowUserPointer(window));
        gw->mouseWheel(w, m, static_cast<int>(xpos),static_cast<int>(ypos));
    }
}

void GlfwWindow::mouseMotionFunc(GLFWwindow* window, double  x, double y)
{
    GlfwWindow* gw = static_cast<GlfwWindow*>(glfwGetWindowUserPointer(window));
    // as the GLUT windows, we call mouseMotion when the mouse is moved
    // WHILE a button is clicked, an mousePassiveMotion when no buttons
    // are clicked.
    // I dont like this setup, and would prefer one motion func regardless
    // of button presses, but the ork/proland apps heavily use both of these
    // two methods, so I chose to implement them for GLFW as well
    
    // "Poll" the mouse buttons (actually not a poll, but queries
    // the caches state from glfw):
    int sl = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    int sm = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
    int sr = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    if (sl==GLFW_PRESS || sm==GLFW_PRESS || sr==GLFW_PRESS) {
        gw->mouseMotion(
            static_cast<int>(x),
            static_cast<int>(y));
    }
    else {
        gw->mousePassiveMotion(
            static_cast<int>(x),
            static_cast<int>(y));

    }
}

void GlfwWindow::mouseEnterLeaveFunc(GLFWwindow* window, int entered)
{
    GlfwWindow* gw = static_cast<GlfwWindow*>(glfwGetWindowUserPointer(window));
    // TO BE IMPLEMENTED
}

void GlfwWindow::keyCallback(GLFWwindow* window, int key, int scancode, int action, int     mods)
{
    // Get modifiers:
    EventHandler::modifier m = getModifiers(window);
    
    // Get the mous cordinates:
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    
    GlfwWindow* gw = static_cast<GlfwWindow*>(glfwGetWindowUserPointer(window));
    
    // Translate the key.
    // If 32 <= key <= 92, we pass through as a char
    if(key >= 32 && key <= 96)
    {
        char c = (unsigned char)key;

        if(action==GLFW_PRESS)
            gw->keyTyped(c, m,
                static_cast<int>(xpos),
                static_cast<int>(ypos));
        // There was a bug here, as the condition below also fired on
        // GLFW_REPEAT (depending on system settings)?
        else if (action==GLFW_RELEASE)
            gw->keyReleased(c, m,
                static_cast<int>(xpos),
                static_cast<int>(ypos));
    } else if(key >= 256 && key <= 348) 
    {
        // GLFWs 256 and up are special keys
        EventHandler::key k = static_cast<EventHandler::key>(key);
    
        if(action==GLFW_PRESS)
            gw->specialKey(k, m,
                static_cast<int>(xpos),
                static_cast<int>(ypos));
        // There was a bug here, as the condition below also fired on
        // GLFW_REPEAT (depending on system settings)?
        else if(action==GLFW_RELEASE)
            gw->specialKeyReleased(k, m,
                static_cast<int>(xpos),
                static_cast<int>(ypos));
    } else
        throw runtime_error("key decoder failed");
}



void GlfwWindow::focusFunc(GLFWwindow* window, int focus)
{
    GlfwWindow* gw = static_cast<GlfwWindow*>(glfwGetWindowUserPointer(window));  
    gw->damaged = (focus != 0);

    // From glfw docs:
    if(focus){
        // Window got focus
    } else
    {
        // Window lost focus
    }
}

void GlfwWindow::errorCallback(int error, const char* message)
{
    throw std::runtime_error(message);
}

EventHandler::modifier GlfwWindow::getModifiers(GLFWwindow* window)
{
    int key1, key2; EventHandler::modifier m = EventHandler::NONE;
    key1 = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
    key2 = glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT);
    if(key1 == GLFW_PRESS || key2 == GLFW_PRESS)
        m = EventHandler::SHIFT;
                    
    key1 = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL);
    key2 = glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL);
    if(key1 == GLFW_PRESS || key2 == GLFW_PRESS)
        m = EventHandler::CTRL;
                                   
    key1 = glfwGetKey(window, GLFW_KEY_LEFT_ALT);
    key2 = glfwGetKey(window, GLFW_KEY_RIGHT_ALT);
    if(key1 == GLFW_PRESS || key2 == GLFW_PRESS)
        m = EventHandler::ALT;

    return m;
}

void    GlfwWindow::waitForVSync(bool wait)
{
    if(wait)
        glfwSwapInterval(1);
    else
        glfwSwapInterval(0);

}

void GlfwWindow::setWindowTitle(const std::string& title) {
    glfwSetWindowTitle((GLFWwindow*)glfwWindowHandle, title.c_str());
}


}
