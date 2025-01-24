#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>

enum Camera_Movement
{
	FORWARD,
	BACK,
	LEFT,
	RIGHT
};

//default camera values
const float defaultYaw = -90.0f;
const float defaultPitch = 0.0f;
const float defaultSpeed = 2.5f;
const float defaultSens = 0.1f;
const float defaultZoom = 45.0f;

class Camera
{
public:
	glm::vec3 m_Position;
	glm::vec3 m_Front;
	glm::vec3 m_Up;
	glm::vec3 m_Right;
	glm::vec3 m_WorldUp;

	float m_Yaw;
	float m_Pitch;

	float m_Speed;
	float m_Sens;
	float m_Zoom;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
		float yaw = defaultYaw,
		float pitch = defaultPitch) :
		m_Front(glm::vec3(0.0f, 0.0f, -1.0f)),
		m_Speed(defaultSpeed),
		m_Sens(defaultSens),
		m_Zoom(defaultZoom)
	{
		m_Position = position;
		m_WorldUp = up;
		m_Yaw = yaw;
		m_Pitch = pitch;
		UpdateCameraVectors();
	}

	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
	}

	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = m_Speed * deltaTime;
		if (direction == FORWARD) { m_Position += m_Front * velocity; }
		if (direction == BACK) { m_Position -= m_Front * velocity; }
		if (direction == LEFT) { m_Position += m_Right * velocity; }
		if (direction == RIGHT) { m_Position -= m_Right * velocity; }
	}

	void ProcessMouseMovement(float xOffset, float yOffset, GLboolean constainPitch = true)
	{
		xOffset *= m_Sens;
		yOffset *= m_Sens;

		if (constainPitch)
		{
			if (m_Pitch > 89.0f)
				m_Pitch = 89.0f;
			if (m_Pitch < -89.0f)
				m_Pitch = -89.0f;
		}

		UpdateCameraVectors();
	}

	void ProcessMouseScroll(float yOffset)
	{
		m_Zoom -= (float)yOffset;
		if (m_Zoom < 1.0f)
			m_Zoom = 1.0f;
		if (m_Zoom > 45.0f)
			m_Zoom = 45.0f;
	}

private:

	//calculate front vector from the camera's updated position
	void UpdateCameraVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		front.y = sin(glm::radians(m_Pitch));
		front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		m_Front = glm::normalize(front);

		//calculate right and up vectors
		m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
		m_Up = glm::normalize(glm::cross(m_Right, m_Front));
	}
};

#endif
