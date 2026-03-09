#pragma once

#include "Aurora/Renderer/Camera.h"

namespace Aurora {

	class OrthographicCamera : public Camera {
	public:
		struct ProjectionProperties {
			float Left = 0, Right = 0, Bottom = 0, Top = 0;
			float Near = -1.0f;
			float Far = 1.0f;
		};
		
		OrthographicCamera(const ProjectionProperties& props);
		
		void SetProjection(const ProjectionProperties& props);

		const glm::vec3& GetPosition() const;
		void SetPosition(const glm::vec3& position);

		float GetRotation() const;
		void SetRotation(float rotation);

		const glm::mat4& GetProjectionMatrix() const;
		const glm::mat4& GetViewMatrix() const;
		const glm::mat4& GetViewProjectionMatrix() const;
		
	private:
		void RecalculateViewMatrix();
		
	private:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;

		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		float m_Rotation = 0.0f;
	};

}

