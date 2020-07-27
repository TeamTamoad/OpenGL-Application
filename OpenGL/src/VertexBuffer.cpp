#include "VertexBuffer.h"
#include "Renderer.h"

VertexBuffer::VertexBuffer() : mID(0)
{
}

VertexBuffer::VertexBuffer(const void* data, GLuint size)
{
    GLCall(glGenBuffers(1, &mID));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, mID));
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

void VertexBuffer::Bind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, mID));

}

void VertexBuffer::Unbind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
