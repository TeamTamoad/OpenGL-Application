#include "VertexBuffer.h"
#include "Renderer.h"

VertexBuffer::VertexBuffer() : mID(0)
{
}

VertexBuffer::VertexBuffer(const void* data, GLuint size)
{
    glGenBuffers(1, &mID);
    glBindBuffer(GL_ARRAY_BUFFER, mID);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void VertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, mID);

}

void VertexBuffer::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
