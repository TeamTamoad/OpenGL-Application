#include "VertexArray.h"

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &mID);
	glBindVertexArray(mID);
}

void VertexArray::AddBuffer(const VertexBuffer& vbo, const VertexLayout& layout)
{
	Bind();
	vbo.Bind();
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for (size_t i = 0; i < elements.size(); ++i)
	{
		const auto& element = elements[i];
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (void*)(offset));
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
}

void VertexArray::Bind() const
{
	glBindVertexArray(mID);
}

void VertexArray::Unbind() const
{
	glBindVertexArray(0);
}

GLuint VertexArray::GetID() const
{
	return mID;
}
