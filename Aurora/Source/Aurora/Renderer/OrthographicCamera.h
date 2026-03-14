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

		const math::Vec3& GetPosition() const;
		void SetPosition(const math::Vec3& position);

		float GetRotation() const;
		void SetRotation(float rotation);

		const math::Mat4& GetProjectionMatrix() const;
		const math::Mat4& GetViewMatrix() const;
		const math::Mat4& GetViewProjectionMatrix() const;
		
	private:
		void RecalculateViewMatrix();
		
	private:
		math::Mat4 m_ProjectionMatrix;
		math::Mat4 m_ViewMatrix;
		math::Mat4 m_ViewProjectionMatrix;

		math::Vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		float m_Rotation = 0.0f;
	};

}

