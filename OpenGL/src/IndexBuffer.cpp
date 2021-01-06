#include "IndexBuffer.h"
#include "Renderer.h"

IndexBuffer::IndexBuffer() : mID(0)
{
}

IndexBuffer::IndexBuffer(const void* data, GLuint size)
{
    glGenBuffers(1, &mID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}


void IndexBuffer::Bind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID);

}

void IndexBuffer::Unbind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
