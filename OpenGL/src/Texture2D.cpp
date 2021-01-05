#include "Texture2D.h"
#include <iostream>
#include <stb_image.h>

Texture2D::Texture2D(const char* source, GLuint slot) : mSlot(slot), mID(0)
{
	int width, height, nrChannels;
	unsigned char* image;

	image = stbi_load(source, &width, &height, &nrChannels, 0);
	if (image)
	{
		GLenum format = GL_RGB;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		GLCall(glGenTextures(1, &mID));
		GLCall(glActiveTexture(GL_TEXTURE0 + mSlot));
		GLCall(glBindTexture(GL_TEXTURE_2D, mID));

		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image));
		GLCall(glGenerateMipmap(GL_TEXTURE_2D));

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "ERROR::TEXTURE2D::LOADING_IMAGE_FAILED" << "\n";
		ASSERT(false);
	}

	stbi_image_free(image);
}

void Texture2D::SetBoarder(GLint paramS, GLint paramT)
{
	glActiveTexture(GL_TEXTURE0 + mSlot);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, paramS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, paramT);
	glActiveTexture(GL_TEXTURE0);
}
