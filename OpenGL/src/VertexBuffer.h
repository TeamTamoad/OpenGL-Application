#pragma once

#include <GL/glew.h>

class VertexBuffer
{
public:
	GLuint mID;

	VertexBuffer();
	VertexBuffer(const void* data, GLuint size);

	void Bind() const;
	void Unbind() const;

};