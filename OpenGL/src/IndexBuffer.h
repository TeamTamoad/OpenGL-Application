#pragma once

#include <GL/glew.h>
#include <vector>

class IndexBuffer
{
public:
	GLuint mID;

	IndexBuffer();
	IndexBuffer(const void* data, GLuint size);
	template<typename T>
	IndexBuffer(const std::vector<T>& data);
	

	void Bind();
	void Unbind();

};

template<typename T>
inline IndexBuffer::IndexBuffer(const std::vector<T>& data)
{
	glGenBuffers(1, &mID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(T), &data[0], GL_STATIC_DRAW);
}
