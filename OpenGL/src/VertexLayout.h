#pragma once
#include <vector>
#include <GL/glew.h>


struct VertexBufferElement
{
	GLuint type;
	unsigned int count;
	unsigned char normalized;

	static unsigned int GetSizeOfType(GLuint type)
	{
		switch (type)
		{
			case GL_FLOAT: return sizeof(float);
		}
		return 0;
	}
};

class VertexLayout
{
private:
	std::vector<VertexBufferElement> mElements;
	unsigned int mStride;

public:
	VertexLayout() : mStride(0)
	{
	}

	template<typename T>
	void Push(unsigned int count)
	{
		static_assert(false);
	}

	template<>
	void Push<float>(unsigned int count)
	{
		mElements.push_back({ GL_FLOAT, count, GL_FALSE });
		mStride += sizeof(GLfloat) * count;
	}

	unsigned int GetStride() const { return mStride; }

	const std::vector<VertexBufferElement>& GetElements() const { return mElements; }


};

