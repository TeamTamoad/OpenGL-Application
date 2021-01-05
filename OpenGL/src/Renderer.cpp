#include "Renderer.h"
#include <iostream>

void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
    if (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] " << "0x" << std::hex << error << std::dec << "\n"
            << function << " " << file << ": " << line << "\n";
        return false;
    }
    return true;
}