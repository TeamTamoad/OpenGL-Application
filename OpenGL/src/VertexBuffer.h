#pragma once

#include <GL/glew.h>
#include <vector>

class VertexBuffer
{
public:
	GLuint mID;

	VertexBuffer();
	VertexBuffer(const void* data, GLuint size);
	template<typename T>
	VertexBuffer(const std::vector<T>& data);

	void Bind() const;
	void Unbind() const;

};

template<typename T>
inline VertexBuffer::VertexBuffer(const std::vector<T>& data)
{
	glGenBuffers(1, &mID);
	glBindBuffer(GL_ARRAY_BUFFER, mID);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), &data[0], GL_STATIC_DRAW);
}
