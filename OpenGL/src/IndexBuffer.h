#pragma once

#include <GL/glew.h>

class IndexBuffer
{
public:
	GLuint mID;

	IndexBuffer();
	IndexBuffer(const void* data, GLuint size);
	

	void Bind();
	void Unbind();

};