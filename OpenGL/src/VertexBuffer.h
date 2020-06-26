#pragma once

#include <GL/glew.h>

class VertexBuffer
{
public:
	GLuint mID;

	VertexBuffer();
	VertexBuffer(const void* data, GLuint size);

	void Bind();
	void Unbind();

};