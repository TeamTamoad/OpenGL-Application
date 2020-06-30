#pragma once

#include "Renderer.h"

class Texture2D
{
public:
	GLuint mID;
	GLuint mSlot;
	Texture2D(const char* source, const GLuint& slot = 0);

	void SetBoarder(GLint S, GLint T);
};