#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum class CameraMovement
{
	FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 1.0f;
const float FOV = 45.0f;

class Camera
{
public:
	// camera Attributes
	glm::vec3 mPosition;
	glm::vec3 mWorldUp;
	glm::vec3 mFront;
	glm::vec3 mUp;
	glm::vec3 mRight;
	// Euler angles
	float mYaw;
	float mPitch;
	// camera options
	float mMovementSpeed	= 2.5f;
	float mMouseSensitivity = 1.0f;
	float mFOV				= 45.0f;
	float mBoostSpeed		= 2.0f;
	// bool options
	bool mBoosted = false;
	bool mFreezed = false;

	// default constructor
	Camera();
	// constructor with vectors
	Camera(const glm::vec3& position, const glm::vec3& up, float yaw, float pitch);
	// constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);
	
	glm::mat4 GetViewMatrix();

	void ProcessKeyboard(CameraMovement direction, float deltaTime);
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
	void ProcessMouseScroll(float yoffset);
	void SetBoostSpeed(float speedUP);
	void updateCameraVectors();
};
