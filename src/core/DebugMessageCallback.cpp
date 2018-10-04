#include "mork/core/DebugMessageCallback.h"
#include "mork/core/Log.h"
#include "mork/glad/glad.h"

#include <sstream>

namespace mork {

void APIENTRY debugMessageCallback(GLenum source, 
                            GLenum type, 
                            GLuint id, 
                            GLenum severity, 
                            GLsizei length, 
                            const GLchar *message, 
                            const void *userParam)
{
    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

    std::stringstream ss;


    //ss << "---------------\n";
    ss << "Debug message (" << id << "): " <<  message << "\n";
    
    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             ss << "   Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   ss << "   Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: ss << "   Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     ss << "   Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     ss << "   Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           ss << "   Source: Other"; break;
    } ss << "\n";

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               ss << "   Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: ss << "   Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  ss << "   Type: Undefined Behaviour"; break; 
        case GL_DEBUG_TYPE_PORTABILITY:         ss << "   Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         ss << "   Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              ss << "   Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          ss << "   Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           ss << "   Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               ss << "   Type: Other"; break;
    } ss << "\n";
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            ss << "   Severity: high";
            error_logger(ss.str());
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            ss << "   Severity: medium";
            warn_logger(ss.str());
            break;
        case GL_DEBUG_SEVERITY_LOW:
            ss << "   Severity: low";
            warn_logger(ss.str());
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            ss << "   Severity: notification";
            info_logger(ss.str());
            break;
    };
    
    // Throw exception on high severity:
    if(severity==GL_DEBUG_SEVERITY_HIGH)
        throw std::runtime_error(ss.str());
}

}
