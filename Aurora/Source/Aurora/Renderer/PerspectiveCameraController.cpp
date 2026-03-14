#include "aupch.h"
#include "PerspectiveCameraController.h"

#include "Aurora/Core/Application.h"
#include "Aurora/Core/Input.h"
#include "Aurora/Core/Log.h"
#include "GLFW/glfw3.h"

namespace Aurora {
	void PerspectiveCameraController::OnUpdate(Timestep ts) {
		if (Input::IsKeyPressed(Key::W)) {
			m_TargetCamera->Walk(m_CameraTranslationSpeed * ts.GetSeconds());
		}
		if (Input::IsKeyPressed(Key::A)) {
			m_TargetCamera->Strafe(-m_CameraTranslationSpeed * ts.GetSeconds());
		}
		if (Input::IsKeyPressed(Key::S)) {
			m_TargetCamera->Walk(-m_CameraTranslationSpeed * ts.GetSeconds());
		}
		if (Input::IsKeyPressed(Key::D)) {
			m_TargetCamera->Strafe(m_CameraTranslationSpeed * ts.GetSeconds());
		}
		if (Input::IsKeyPressed(Key::Space)) {
			m_TargetCamera->Rise(m_CameraTranslationSpeed * ts.GetSeconds());
		}
		if (Input::IsKeyPressed(Key::LeftShift)) {
			m_TargetCamera->Rise(-m_CameraTranslationSpeed * ts.GetSeconds());
		}

		m_TargetCamera->UpdateViewMatrix();

		// -------------------------

		//float dt = ts.GetSeconds();
		//DirectX::XMVECTOR accelDir = DirectX::XMVectorZero();

		//if (Input::IsKeyPressed(Key::W)) accelDir = DirectX::XMVectorAdd(accelDir, m_TargetCamera->GetLook());
		//if (Input::IsKeyPressed(Key::S)) accelDir = DirectX::XMVectorSubtract(accelDir, m_TargetCamera->GetLook());
		//if (Input::IsKeyPressed(Key::A)) accelDir = DirectX::XMVectorSubtract(accelDir, m_TargetCamera->GetRight());
		//if (Input::IsKeyPressed(Key::D)) accelDir = DirectX::XMVectorAdd(accelDir, m_TargetCamera->GetRight());
		//if (Input::IsKeyPressed(Key::Space)) accelDir = DirectX::XMVectorAdd(accelDir, m_TargetCamera->GetUp());
		//if (Input::IsKeyPressed(Key::LeftShift)) accelDir = DirectX::XMVectorSubtract(accelDir, m_TargetCamera->GetUp());

		//accelDir = DirectX::XMVector3Normalize(accelDir);

		//DirectX::XMVECTOR velocity = XMLoadFloat3(&m_CurrentVelocity);

		//velocity = DirectX::XMVectorMultiplyAdd(accelDir, DirectX::XMVectorReplicate(m_Acceleration * dt), velocity);

		//float drag = 1.0f - (m_Damping * dt);
		//drag = std::max(drag, 0.0f);
		//velocity = DirectX::XMVectorScale(velocity, drag);

		//float speedSq = DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(velocity));
		//if (speedSq > m_CameraTranslationSpeed * m_CameraTranslationSpeed) {
		//	velocity = DirectX::XMVectorScale(DirectX::XMVector3Normalize(velocity), m_CameraTranslationSpeed);
		//}

		//XMStoreFloat3(&m_CurrentVelocity, velocity);

		//DirectX::XMVECTOR pos = m_TargetCamera->GetPosition();
		//pos = DirectX::XMVectorMultiplyAdd(velocity, DirectX::XMVectorReplicate(dt), pos);

		//DirectX::XMFLOAT3 posS;
		//DirectX::XMStoreFloat3(&posS, pos);

		//m_TargetCamera->SetPosition(posS);

		//m_TargetCamera->UpdateViewMatrix();
	}

	void PerspectiveCameraController::OnEvent(Event& e) {
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(PerspectiveCameraController::OnWindowResize));
		dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(PerspectiveCameraController::OnMouseMove));
		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(PerspectiveCameraController::OnKeyPress));
	}

	void PerspectiveCameraController::OnResize(float width, float height) {
		m_TargetCamera->SetLens(0.25f * MathHelper::Pi, AspectRatio(width, height), 1.f, 1000.f);
	}

	PerspectiveCamera& PerspectiveCameraController::GetCamera() {
		return *m_TargetCamera;
	}

	const PerspectiveCamera& PerspectiveCameraController::GetCamera() const {
		return *m_TargetCamera;
	}

	bool PerspectiveCameraController::OnWindowResize(WindowResizeEvent& e) {
		OnResize(static_cast<float>(e.GetWidth()), static_cast<float>(e.GetHeight()));
		return false;
	}

	bool PerspectiveCameraController::OnKeyPress(KeyPressedEvent& e) {
		if (e.GetKeyCode() == Key::Escape && !e.IsRepeat()) {
			m_IsCursorDisabled = !m_IsCursorDisabled;
			// TODO: put cursor disabling to window
			auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
			if (m_IsCursorDisabled) {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				Application::Get().ImGuiBlockEvents(false);
				double x = 0, y = 0;
				glfwGetCursorPos(window, &x, &y);
				m_LastMousePosition = DirectX::XMFLOAT2(static_cast<float>(x), static_cast<float>(y));
			} else {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				Application::Get().ImGuiBlockEvents(true);
			}
		}
		return false;
	}

	bool PerspectiveCameraController::OnMouseMove(MouseMovedEvent& e) {
		if (m_IsCursorDisabled) {
			//float deltaX = (e.GetX() - m_LastMousePosition.x) * DirectX::XMConvertToRadians(m_CameraRotationSpeed);
			//float deltaY = (e.GetY() - m_LastMousePosition.y) * DirectX::XMConvertToRadians(m_CameraRotationSpeed);

			//m_Yaw += deltaX;
			//m_Pitch += deltaY;

			//m_Pitch = ClampPitch(m_Pitch);
			//m_Yaw = fmod(m_Yaw, 360.0f);

			//if (deltaX != 0.0f) m_TargetCamera->Yaw(deltaX);
			//if (deltaY != 0.0f) m_TargetCamera->Pitch(deltaY);

			//m_LastMousePosition.x = e.GetX();
			//m_LastMousePosition.y = e.GetY();
			// -----------------

			//float lastPitch = m_Pitch;
			//float lastYaw = m_Yaw;

			//m_Yaw += (m_LastMousePosition.x - e.GetX()) * DirectX::XMConvertToRadians(m_CameraRotationSpeed);
			//m_Pitch += (m_LastMousePosition.y - e.GetY()) * DirectX::XMConvertToRadians(m_CameraRotationSpeed);

			//m_Pitch = ClampPitch(m_Pitch);

			//m_Yaw = fmod(m_Yaw, 360.0f);
			//if (m_Yaw < 0.0f) m_Yaw += 360.0f;

			//if (lastPitch != m_Pitch || lastYaw != m_Yaw) {
			//	m_TargetCamera->Pitch(m_Pitch);
			//	m_TargetCamera->Yaw(m_Yaw);
			//}

			//m_LastMousePosition.x = e.GetX();
			//m_LastMousePosition.y = e.GetY();

			// ------------------------------

			float dx = e.GetX() - m_LastMousePosition.x;
			float dy = e.GetY() - m_LastMousePosition.y;

			m_Yaw -= dx * m_CameraRotationSpeed;
			m_Pitch -= dy * m_CameraRotationSpeed;

			m_Pitch = ClampPitch(m_Pitch);

			m_TargetCamera->SetRotation(
				DirectX::XMConvertToRadians(m_Pitch),
				DirectX::XMConvertToRadians(m_Yaw)
			);

			m_LastMousePosition.x = e.GetX();
			m_LastMousePosition.y = e.GetY();
		}
		return false;
	}
}
