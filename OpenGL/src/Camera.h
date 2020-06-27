#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum CameraMovement
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
	glm::vec3 mFront;
	glm::vec3 mUp;
	glm::vec3 mRight;
	glm::vec3 mWorldUp;
	// Euler angles
	float mYaw;
	float mPitch;
	// camera options
	float mMovementSpeed;
	float mMouseSensitivity;
	float mFOV;
	// bool options
	bool mDoubleSpeed;

	// constructor with vectors
	Camera(const glm::vec3& position = glm::vec3(0.0f), const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f), const float& yaw = YAW, const float& pitch = PITCH);
	// constructor with scalar values
	Camera(const float& posX, const float& posY, const float& posZ, const float& upX, const float& upY, const float& upZ, const float& yaw, const float& pitch);
	
	glm::mat4 GetViewMatrix();

	void ProcessKeyboard(CameraMovement direction, float deltaTime);
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
	void ProcessMouseScroll(float yoffset);

private:
	void updateCameraVectors();
};
