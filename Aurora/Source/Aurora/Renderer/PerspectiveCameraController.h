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
		PerspectiveCameraController() = default;
		virtual ~PerspectiveCameraController() = default;

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		void OnResize(float width, float height);

		// TODO: make it safe
		PerspectiveCamera& GetCamera();
		const PerspectiveCamera& GetCamera() const;

		void SetTarget(PerspectiveCamera* camera) {
			m_TargetCamera = camera;
			//m_TargetTransform = transform;
		}

		//glm::vec3& GetCameraPosition();
		//const glm::vec3& GetCameraPosition() const;
		//glm::vec3 GetCameraRotation() const;

	private:
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnKeyPress(KeyPressedEvent& e);
		bool OnMouseMove(MouseMovedEvent& e);

		static float ClampPitch(float pitch) { return std::clamp(pitch, -89.9f, 89.9f); }
		static float AspectRatio(float width, float height) { return width / height; }

	private:
		PerspectiveCamera* m_TargetCamera;
		//TransformComponent* m_TargetTransform = nullptr;

		float m_Yaw = 90.0f;
		float m_Pitch = 0.0f;

		float m_CameraTranslationSpeed = 5.0f;
		float m_CameraRotationSpeed = 0.1f;

		DirectX::XMFLOAT3 m_CurrentVelocity = { 0.0f, 0.0f, 0.0f };
		float m_Acceleration = 50.0f;
		float m_Damping = 10.0f;

		DirectX::XMFLOAT2 m_LastMousePosition = { 0.0f, 0.0f };
		bool m_IsCursorDisabled = false;
	};
	
}
