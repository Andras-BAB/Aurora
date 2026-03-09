#include "aupch.h"
#include "PerspectiveCamera.h"

#include <glm/ext/matrix_clip_space.hpp>

namespace Aurora {
	PerspectiveCamera::PerspectiveCamera(const glm::vec3& position, const glm::vec3& up, float aspectRatio)
		: m_Position(position), m_CameraUp(up), m_AspectRatio(aspectRatio) {
		RecalculateVectors();
		RecalculateViewProjectionMatrix();
	}

	void PerspectiveCamera::SetProjection(const float aspectRatio) {
		m_AspectRatio = aspectRatio;
		RecalculateViewProjectionMatrix();
	}

	void PerspectiveCamera::SetFOV(const float fov) {
		m_FOV = fov;
		RecalculateProjectionMatrix();
	}

	const glm::vec3& PerspectiveCamera::GetPosition() const {
		return m_Position;
	}

	void PerspectiveCamera::SetPosition(const glm::vec3& position) {
		m_Position = position;
		RecalculateViewProjectionMatrix();
	}

	void PerspectiveCamera::SetRotation(const float yaw, const float pitch) {
		m_Yaw = yaw;
		m_Pitch = pitch;
		RecalculateViewProjectionMatrix();
	}

	const glm::mat4& PerspectiveCamera::GetProjectionMatrix() const {
		return m_ProjectionMatrix;
	}

	const glm::mat4& PerspectiveCamera::GetViewMatrix() const {
		return m_ViewMatrix;
	}

	const glm::mat4& PerspectiveCamera::GetViewProjectionMatrix() const {
		return m_ViewProjectionMatrix;
	}

	glm::vec3 PerspectiveCamera::GetFront() const {
		return m_Front;
	}

	glm::vec3 PerspectiveCamera::GetUp() const {
		return m_CameraUp;
	}

	glm::vec3 PerspectiveCamera::GetRight() const {
		return m_Right;
	}

	float PerspectiveCamera::GetFOV() const {
		return m_FOV;
	}

	void PerspectiveCamera::RecalculateVectors() {
		glm::vec3 front;
		front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		front.y = sin(glm::radians(m_Pitch));
		front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		m_Front = glm::normalize(front);
		
		m_Right = glm::normalize(glm::cross(m_WorldUp, m_Front));
		m_CameraUp = glm::normalize(glm::cross(m_Front, m_Right));
	}

	void PerspectiveCamera::RecalculateViewMatrix() {
		RecalculateVectors();
		m_ViewMatrix = glm::lookAtLH(m_Position, m_Position + m_Front, m_CameraUp);
	}

	void PerspectiveCamera::RecalculateProjectionMatrix() {
		m_ProjectionMatrix = glm::perspectiveLH(m_FOV, m_AspectRatio, m_Near, m_Far);
	}

	void PerspectiveCamera::RecalculateViewProjectionMatrix() {
		RecalculateViewMatrix();
		RecalculateProjectionMatrix();
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}
