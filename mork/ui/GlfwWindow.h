#ifndef _MORK_GLFW_WINDOW_H_
#define _MORK_GLFW_WINDOW_H_

#include <map>

#include "mork/glad/glad.h"
#include <GLFW/glfw3.h>

#include "mork/ui/Window.h"
#include "mork/math/vec2.h"
#include "mork/core/Timer.h"

namespace mork
{

/**
 * A Window implemented using glfw.
 * (c) 2016 Lars Flæten
 */
class GlfwWindow : public Window
{
public:
    /**
     * Creates a new window. If the window size is 0,0 a full screen window
     * is created.
     *
     * @param params the parameters of the window.
     */
    GlfwWindow(const Window::Parameters &params);

    /**
     * Deletes this window.
     */
    virtual ~GlfwWindow();

    virtual int getWidth() const;

    virtual int getHeight() const;

    virtual void start();

    virtual void redisplay(double t, double dt);

    virtual void reshape(int x, int y);

    virtual void idle(bool damaged);

    // FPS for the last second
    virtual double getFps() const;

    // Average frametime for the last second
    virtual double getFrameTime() const;

    // return cirrent frame count
    virtual unsigned int getFrameCount() const;

    // frametime for the current frame
    virtual double getDt() const;

    virtual void fpsUpdatedEvent();

    virtual void setWindowTitle(const std::string& title);

    /**
     * Tells the windowing system wether to wait for a vertical
     * sync or not before swapping buffers.
     */
    void    waitForVSync(bool wait);

    /**
     * Returns the current mouse cursor position
     */
    void    getMousePosition(int* x, int* y);

    void    shouldClose();

    static bool isContextActive();


protected:
    /**
     * The Window instances. Maps window id to Window instances.
     */
    //static std::map<int, GlfwWindow*> INSTANCES;

    /**
     * The id of this window.
     */
    int windowId;

    /**
     * The glfw window handle
     */
    void* glfwWindowHandle;

    /**
     * The current size of this window.
     */
    vec2i size;

    /**
     * True if this windows just gained focus.
     */
    bool damaged;

    /**
     * Timer used for computing the parameters of redisplay.
     */
    Timer timer;

    /**
     * The time at the end of the last execution of #redisplay.
     */
    double t;

    /**
     * The elapsed time bewteen the two previous calls to #redisplay.
     */
    double dt;

    /**
     * Frame counter/fps related variables
     */
    double fps;
    unsigned int frameCount;
    double fps_t0;
    /**
     * Glfw callback that calls #redisplay on the active Window.
     */
    static void redisplayFunc();

    /**
     * Glfw callback that calls #reshape on the active Window.
     */
    static void reshapeFunc(GLFWwindow*, int w, int h);

    /**
     * Glfw callback that calls #mouseClick on the active Window.
     */
    static void mouseClickFunc(GLFWwindow* window, int button, int action, int mods);

    /**
     *
     */
    static void scrollFunc(GLFWwindow* window, double xScroll, double yScroll);
    
    /**
     * Glfw callback that calls #mouseMotion on the active Window.
     */
    static void mouseMotionFunc(GLFWwindow* window, double x, double y);

    /**
     * Glfw callback that calls #mouseEnterLeave on the active Window.
     */
    static void mouseEnterLeaveFunc(GLFWwindow* window, int entered);

    /**
     * Key callback
     */
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    /**
     * Glfw callback for focus events.
     */
    static void focusFunc(GLFWwindow* window, int focus);

    /**
     * Glfw error callback.
     */
    static void errorCallback(int error, const char* message);

    /**
     * Utility function to check wether shift, ctrl or alt
     * has been pressed
     */
    static EventHandler::modifier getModifiers(GLFWwindow* wd);


};

}

#endif
