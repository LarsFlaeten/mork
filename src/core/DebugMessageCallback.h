#ifndef _MORK_DEBUGMESSAGECALLBACK_H_
#define _MORK_DEBUGMESSAGECALLBACK_H_



#include "mork/glad/glad.h"

namespace mork {

void APIENTRY debugMessageCallback(GLenum source, 
                            GLenum type, 
                            GLuint id, 
                            GLenum severity, 
                            GLsizei length, 
                            const GLchar *message, 
                            const void *userParam);
}

#endif
