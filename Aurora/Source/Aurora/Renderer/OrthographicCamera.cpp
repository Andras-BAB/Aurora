#include "aupch.h"
#include "OrthographicCamera.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <spdlog/pattern_formatter.h>
#include <spdlog/fmt/bundled/core.h>

namespace Aurora {
	OrthographicCamera::OrthographicCamera(const ProjectionProperties& props)
		: m_ProjectionMatrix(glm::ortho(props.Left, props.Right, props.Bottom, props.Top, props.Near, props.Far)), m_ViewMatrix(1.0f) {

		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::RecalculateViewMatrix() {
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
			glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::SetProjection(const ProjectionProperties& props) {
		m_ProjectionMatrix = glm::ortho(props.Left, props.Right, props.Bottom, props.Top, props.Near, props.Far);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	const glm::vec3& OrthographicCamera::GetPosition() const {
		return m_Position;
	}

	void OrthographicCamera::SetPosition(const glm::vec3& position) {
		m_Position = position;
		RecalculateViewMatrix();
	}

	float OrthographicCamera::GetRotation() const {
		return m_Rotation;
	}

	void OrthographicCamera::SetRotation(float rotation) {
		m_Rotation = rotation;
		RecalculateViewMatrix();
	}
	
	const glm::mat4& OrthographicCamera::GetProjectionMatrix() const {
		return m_ProjectionMatrix;
	}

	const glm::mat4& OrthographicCamera::GetViewMatrix() const {
		return m_ViewMatrix;
	}

	const glm::mat4& OrthographicCamera::GetViewProjectionMatrix() const {
		return m_ViewProjectionMatrix;
	}
	
}
