#include "Camera.h"

Camera::Camera(const glm::vec3& position, const glm::vec3& up, const float& yaw, const float& pitch)
	: mPosition(position), mWorldUp(up), mFront(glm::vec3(0.0f, 0.0f, -1.0f)), mYaw(yaw), mPitch(pitch), 
	  mMovementSpeed(SPEED), mMouseSensitivity(SENSITIVITY), mFOV(FOV), mBoosted(false), mBoostSpeed(2)
{
	updateCameraVectors();
}

Camera::Camera(const float& posX, const float& posY, const float& posZ, const float& upX, const float& upY, const float& upZ, const float& yaw, const float& pitch)
	: mFront(glm::vec3(0.0f, 0.0f, -1.0f)), mYaw(yaw), mPitch(pitch), mMovementSpeed(SPEED), 
	  mMouseSensitivity(SENSITIVITY), mFOV(FOV), mBoosted(false), mBoostSpeed(2)
{
	mPosition = glm::vec3(posX, posY, posZ);
	mWorldUp = glm::vec3(upX, upY, upZ);
	updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(mPosition, mPosition + mFront, mUp);
}

void Camera::ProcessKeyboard(CameraMovement direction, float deltaTime)
{
	float velocity = mMovementSpeed * deltaTime;
	if (mBoosted) velocity *= mBoostSpeed;

	if (direction == CameraMovement::FORWARD)
		mPosition += mFront * velocity;
	if (direction == CameraMovement::BACKWARD)
		mPosition -= mFront * velocity;
	if (direction == CameraMovement::LEFT)
		mPosition -= mRight * velocity;
	if (direction == CameraMovement::RIGHT)
		mPosition += mRight * velocity;
	if (direction == CameraMovement::UP)
		mPosition += mWorldUp * velocity;
	if (direction == CameraMovement::DOWN)
		mPosition -= mWorldUp * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
	xoffset *= mMouseSensitivity;
	yoffset *= mMouseSensitivity;

	mYaw += xoffset;
	mPitch += yoffset;

	if (constrainPitch)
	{
		mPitch = mPitch >  89.0f ?  89.0f : mPitch;
		mPitch = mPitch < -89.0f ? -89.0f : mPitch;
	}

	updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
	mFOV -= (float)yoffset;
	mFOV = mFOV > 45.0f ? 45.0f : mFOV;
	mFOV = mFOV <  1.0f ?  1.0f : mFOV;
}

void Camera::SetBoostSpeed(float speedUP)
{
	mBoostSpeed = speedUP;
}

void Camera::updateCameraVectors()
{
	glm::vec3  front;
	front.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
	front.y = sin(glm::radians(mPitch));
	front.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));

	mFront = glm::normalize(front);
	mRight = glm::normalize(glm::cross(mFront, mWorldUp));
	mUp    = glm::normalize(glm::cross(mRight, mFront));
}


