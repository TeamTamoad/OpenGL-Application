#include "Camera.h"

Camera::Camera(const glm::vec3& position, const glm::vec3& up, const float& yaw, const float& pitch)
	: m_Position(position), m_WorldUp(up), m_Front(glm::vec3(0.0f, 0.0f, -1.0f)), m_Yaw(yaw), m_Pitch(pitch), 
	  m_MovementSpeed(SPEED), m_MouseSensitivity(SENSITIVITY), m_FOV(FOV)
{
	updateCameraVectors();
}

Camera::Camera(const float& posX, const float& posY, const float& posZ, const float& upX, const float& upY, const float& upZ, const float& yaw, const float& pitch)
	: m_Front(glm::vec3(0.0f, 0.0f, -1.0f)), m_Yaw(yaw), m_Pitch(pitch), m_MouseSensitivity(SENSITIVITY), m_FOV(FOV)
{
	m_Position = glm::vec3(posX, posY, posZ);
	m_WorldUp = glm::vec3(upX, upY, upZ);
	updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

void Camera::ProcessKeyboard(CameraMovement direction, float deltaTime)
{
	float velocity = m_MovementSpeed * deltaTime;
	if (direction == FORWARD)
		m_Position += m_Front * velocity;
	if (direction == BACKWARD)
		m_Position -= m_Front * velocity;
	if (direction == LEFT)
		m_Position -= m_Right * velocity;
	if (direction == RIGHT)
		m_Position += m_Right * velocity;
	if (direction == UP)
		m_Position += m_WorldUp * velocity;
	if (direction == DOWN)
		m_Position -= m_WorldUp * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
	xoffset *= m_MouseSensitivity;
	yoffset *= m_MouseSensitivity;

	m_Yaw += xoffset;
	m_Pitch += yoffset;

	if (constrainPitch)
	{
		m_Pitch = m_Pitch >  89.0f ?  89.0f : m_Pitch;
		m_Pitch = m_Pitch < -89.0f ? -89.0f : m_Pitch;
	}

	updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
	m_FOV -= (float)yoffset;
	m_FOV = m_FOV > 45.0f ? 45.0f : m_FOV;
	m_FOV = m_FOV <  1.0f ?  1.0f : m_FOV;
}

void Camera::updateCameraVectors()
{
	glm::vec3  front;
	front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	front.y = sin(glm::radians(m_Pitch));
	front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));

	m_Front = glm::normalize(front);
	m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
	m_Up    = glm::normalize(glm::cross(m_Right, m_Front));
}


