#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>

#include "Camera.h"

class GalaxyAdventure
{
public:
	GalaxyAdventure();
	int run();
	
	static Camera mCamera;
	static float mScrWidth;
	static float mScrHeight;
	static float lastX;
	static float lastY;
	static bool firstMouse;

	
	void ProcessInput(GLFWwindow* window, float deltaTime);
	std::vector<glm::mat4> prepareRockData(int amount, double seed);

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xPos, double yPos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};


