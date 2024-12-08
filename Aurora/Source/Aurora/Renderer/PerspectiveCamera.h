#pragma once
#include "Camera.h"

namespace Aurora {
	
	class PerspectiveCamera : public Camera {
	public:
		explicit PerspectiveCamera(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 3.0f),
			const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f),
			float aspectRatio = 16.0f / 9.0f);

		virtual ~PerspectiveCamera() override = default;

		void SetProjection(float aspectRatio);
		void SetFOV(float fov);
		const glm::vec3& GetPosition() const;
		void SetPosition(const glm::vec3& position);
		void SetRotation(float yaw, float pitch);
		
		const glm::mat4& GetProjectionMatrix() const;
		const glm::mat4& GetViewMatrix() const;
		const glm::mat4& GetViewProjectionMatrix() const;

		glm::vec3 GetFront() const;
		glm::vec3 GetUp() const;
		glm::vec3 GetRight() const;
		float GetFOV() const;
		
	private:
		void RecalculateVectors();		
		void RecalculateViewMatrix();
		void RecalculateProjectionMatrix();
		void RecalculateViewProjectionMatrix();
		
	private:
		glm::mat4 m_ViewMatrix{};
		glm::mat4 m_ViewProjectionMatrix{};

		glm::vec3 m_Front{};
		glm::vec3 m_Right{};
		glm::vec3 m_WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 m_Position;
		glm::vec3 m_CameraUp;
		
		float m_Yaw = 90.0f;
		float m_Pitch = 0.0f;
		float m_FOV = 45.0f;
		float m_Near = 0.1f;
		float m_Far = 100.0f;
		float m_AspectRatio = 16.0f / 9.0f;
		friend class PerspectiveCameraController;
	};
	
}

