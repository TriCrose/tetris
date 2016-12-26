#define GLM_FORCE_INLINE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "CCamera.h"

CCamera::CCamera() {
	m_Position = glm::vec3(0.0f);
	m_fPitch = 0.0f;
	m_fYaw = 0.0f;
}

CCamera::CCamera(glm::vec3 Position) {
	m_Position = Position;
	m_fPitch = 0.0f;
	m_fYaw = 0.0f;
}

glm::mat4 CCamera::GetMatrix() {
	glm::mat4 matrix;
	matrix = glm::rotate(matrix, glm::radians(-m_fPitch), glm::vec3(1.0f, 0.0f, 0.0f));
	matrix = glm::rotate(matrix, glm::radians(m_fYaw), glm::vec3(0.0f, 1.0f, 0.0f));
	matrix = glm::translate(matrix, -m_Position);
	return matrix;
}

void CCamera::LookAt(glm::vec3 Location) {
    float deltaX = Location.x - m_Position.x;
    float deltaY = Location.y - m_Position.y;
    float deltaZ = Location.z - m_Position.z;
    if (deltaZ >= 0) SetYaw(180.0f - glm::degrees(atan((deltaX)/(deltaZ))));
    else SetYaw(-glm::degrees(atan((deltaX)/(deltaZ))));
    SetPitch(glm::degrees(atan((deltaY)/(sqrt(deltaX*deltaX + deltaZ*deltaZ)))));
}
