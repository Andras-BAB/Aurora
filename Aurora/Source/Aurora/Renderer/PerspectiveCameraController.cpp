#include "aupch.h"
#include "PerspectiveCameraController.h"

#include "Aurora/Core/Application.h"
#include "Aurora/Core/Input.h"
#include "Aurora/Core/Log.h"
#include "GLFW/glfw3.h"

namespace Aurora {
	PerspectiveCameraController::PerspectiveCameraController()  : m_Camera(m_CameraPosition) {
		m_Camera.SetPosition(m_CameraPosition);
	}

	void PerspectiveCameraController::OnUpdate(Timestep ts) {
		if (Input::IsKeyPressed(Key::W)) {
			m_CameraPosition += m_Camera.GetFront() * m_CameraTranslationSpeed * ts.GetSeconds();
			m_IsPositionDirty = true;
		}
		if (Input::IsKeyPressed(Key::A)) {
			m_CameraPosition -= m_Camera.GetRight() * m_CameraTranslationSpeed * ts.GetSeconds();
			m_IsPositionDirty = true;
		}
		if (Input::IsKeyPressed(Key::S)) {
			m_CameraPosition -= m_Camera.GetFront() * m_CameraTranslationSpeed * ts.GetSeconds();
			m_IsPositionDirty = true;
		}
		if (Input::IsKeyPressed(Key::D)) {
			m_CameraPosition += m_Camera.GetRight() * m_CameraTranslationSpeed * ts.GetSeconds();
			m_IsPositionDirty = true;
		}
		if (Input::IsKeyPressed(Key::Space)) {
			m_CameraPosition.y += m_CameraTranslationSpeed * ts.GetSeconds();
			m_IsPositionDirty = true;
		}
		if (Input::IsKeyPressed(Key::LeftShift)) {
			m_CameraPosition.y -= m_CameraTranslationSpeed * ts.GetSeconds();
			m_IsPositionDirty = true;
		}

		if (m_IsPositionDirty) {
			m_Camera.SetPosition(m_CameraPosition);
		}
	}

	void PerspectiveCameraController::OnEvent(Event& e) {
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(PerspectiveCameraController::OnWindowResize));
		dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(PerspectiveCameraController::OnMouseMove));
		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(PerspectiveCameraController::OnKeyPress));
	}

	void PerspectiveCameraController::OnResize(float width, float height) {
		m_Camera.SetProjection(width / height);
	}

	PerspectiveCamera& PerspectiveCameraController::GetCamera() {
		return m_Camera;
	}

	const PerspectiveCamera& PerspectiveCameraController::GetCamera() const {
		return m_Camera;
	}

	glm::vec3& PerspectiveCameraController::GetCameraPosition() {
		return m_CameraPosition;
	}

	const glm::vec3& PerspectiveCameraController::GetCameraPosition() const {
		return m_CameraPosition;
	}

	glm::vec3 PerspectiveCameraController::GetCameraRotation() const {
		return { 0.f, m_Pitch, m_Yaw };
	}

	bool PerspectiveCameraController::OnWindowResize(WindowResizeEvent& e) {
		OnResize(static_cast <float>(e.GetWidth()), static_cast <float>(e.GetHeight()));
		return false;
	}

	bool PerspectiveCameraController::OnKeyPress(KeyPressedEvent& e) {
		if (e.GetKeyCode() == Key::Escape && !e.IsRepeat()) {
			m_IsCursorDisabled = !m_IsCursorDisabled;
			// TODO: put cursor disabling to window
			auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
			if (m_IsCursorDisabled) {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				double x = 0, y = 0;
				glfwGetCursorPos(window, &x, &y);
				m_LastMousePosition = glm::vec2(x, y);
			} else {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
		}
		return false;
	}

	bool PerspectiveCameraController::OnMouseMove(MouseMovedEvent& e) {
		if (m_IsCursorDisabled) {
			m_Yaw += (m_LastMousePosition.x - e.GetX()) * m_CameraRotationSpeed;
			m_Pitch += (m_LastMousePosition.y - e.GetY()) * m_CameraRotationSpeed;

			if (m_Pitch > 89.9f) {
				m_Pitch = 89.9f;
			} else if (m_Pitch < -89.9f) {
				m_Pitch = -89.9f;
			}
			
			m_Camera.SetRotation(m_Yaw, m_Pitch);
			m_LastMousePosition.x = e.GetX();
			m_LastMousePosition.y = e.GetY();
		}
		return false;
	}
}
