#pragma once
#include <GL/glew.h>
#include "VertexBuffer.h"
#include "VertexLayout.h"

class VertexArray
{
private:
	GLuint mID;
	
public:
	VertexArray();

	void AddBuffer(const VertexBuffer& vbo, const VertexLayout& layout);

	void Bind() const;
	void Unbind() const;

	GLuint GetID() const;
};

