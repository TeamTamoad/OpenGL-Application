#include "IndexBuffer.h"
#include "Renderer.h"

IndexBuffer::IndexBuffer() : mID(0)
{
}

IndexBuffer::IndexBuffer(const void* data, GLuint size)
{
    GLCall(glGenBuffers(1, &mID));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}


void IndexBuffer::Bind()
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID));

}

void IndexBuffer::Unbind()
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
