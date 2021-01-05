#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <vector>
#include <string>

// Sources sorted by Right, Left, Top, Bottom, Front and Back
GLuint loadCubeMap(const std::vector<std::string>& faces);