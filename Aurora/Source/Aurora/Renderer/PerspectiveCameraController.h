#pragma once
#include "PerspectiveCamera.h"
#include "Aurora/Core/Timestep.h"
#include "Aurora/Events/ApplicationEvent.h"
#include "Aurora/Events/Event.h"
#include "Aurora/Events/KeyEvent.h"
#include "Aurora/Events/MouseEvent.h"

namespace Aurora {

	class PerspectiveCameraController {
	public:
		PerspectiveCameraController();
		virtual ~PerspectiveCameraController() = default;
		
		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		void OnResize(float width, float height);

		PerspectiveCamera& GetCamera();
		const PerspectiveCamera& GetCamera() const;

		glm::vec3& GetCameraPosition();
		const glm::vec3& GetCameraPosition() const;
		glm::vec3 GetCameraRotation() const;
		
	private:
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnKeyPress(KeyPressedEvent& e);
		bool OnMouseMove(MouseMovedEvent& e);

	private:
		PerspectiveCamera m_Camera;
		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, -3.0f };
		float m_Yaw = 90.0f;
		float m_Pitch = 0.0f;
		
		bool m_IsPositionDirty = false;
		float m_CameraTranslationSpeed = 5.0f;
		float m_CameraRotationSpeed = 0.1f;
		
		glm::vec2 m_LastMousePosition = { 0.0f, 0.0f };
		bool m_IsCursorDisabled = false;
	};
	
}
