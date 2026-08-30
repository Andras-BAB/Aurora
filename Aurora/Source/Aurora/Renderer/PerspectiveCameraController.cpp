#include "aupch.h"
#include "PerspectiveCameraController.h"

#include <tracy/Tracy.hpp>

#include "imgui.h"
#include "Aurora/Core/Application.h"
#include "Aurora/Core/Input.h"
#include "Aurora/Core/Log.h"
#include "GLFW/glfw3.h"

namespace Aurora {
	void PerspectiveCameraController::OnUpdate(Timestep ts) {
		ZoneScoped;
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());

		if (m_TargetWindow) {
			window = static_cast<GLFWwindow*>(m_TargetWindow);
		}
		//bool isRightMousePressed = Input::IsMouseButtonPressed(Mouse::ButtonRight);

		if (m_IsRightMousePressed && (m_IsActive || m_IsCursorDisabled)) {

			if (!m_IsCursorDisabled) {
				double x, y;
				glfwGetCursorPos(window, &x, &y);
				m_InitialMousePosition = { (float)x, (float)y };
				m_LastMousePosition = { (float)x, (float)y };

				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

				if (glfwRawMouseMotionSupported()) {
					glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
				}

				//Application::Get().ImGuiBlockEvents(false);
				ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
				m_IsCursorDisabled = true;

				m_RestoreMouseFrames = 0;
			}

			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);

			float dx = (float)xpos - m_LastMousePosition.x;
			float dy = (float)ypos - m_LastMousePosition.y;
			m_LastMousePosition = { (float)xpos, (float)ypos };

			if (dx != 0.0f || dy != 0.0f) {
				m_Yaw += dx * m_CameraRotationSpeed;
				m_Pitch += dy * m_CameraRotationSpeed;

				m_Pitch = ClampPitch(m_Pitch);

				m_TargetCamera->SetRotation(
					0.0f,
					math::Radians(m_Pitch),
					math::Radians(m_Yaw)
				);
			}

			float currentSpeed = m_CameraTranslationSpeed;

			if (Input::IsKeyPressed(Key::LeftShift, window)) {
				currentSpeed *= 4.0f;
			}

			if (Input::IsKeyPressed(Key::W, window)) m_TargetCamera->Walk(currentSpeed * ts.GetSeconds());
			if (Input::IsKeyPressed(Key::S, window)) m_TargetCamera->Walk(-currentSpeed * ts.GetSeconds());
			if (Input::IsKeyPressed(Key::A, window)) m_TargetCamera->Strafe(-currentSpeed * ts.GetSeconds());
			if (Input::IsKeyPressed(Key::D, window)) m_TargetCamera->Strafe(currentSpeed * ts.GetSeconds());
			if (Input::IsKeyPressed(Key::Space, window)) m_TargetCamera->Rise(currentSpeed * ts.GetSeconds());
			if (Input::IsKeyPressed(Key::LeftControl, window)) m_TargetCamera->Rise(-currentSpeed * ts.GetSeconds());

		} else {
			if (m_IsCursorDisabled) {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

				if (glfwRawMouseMotionSupported()) {
					glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
				}

				glfwSetCursorPos(window, m_InitialMousePosition.x, m_InitialMousePosition.y);

				m_IsCursorDisabled = false;

				m_RestoreMouseFrames = 2;
			}
		}

		if (m_RestoreMouseFrames > 0) {
			m_RestoreMouseFrames--;
			if (m_RestoreMouseFrames == 0) {
				//Application::Get().ImGuiBlockEvents(true);
				ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
			}
		}

		m_TargetCamera->UpdateViewMatrix();
	}

	void PerspectiveCameraController::OnEvent(Event& e) {
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(PerspectiveCameraController::OnWindowResize));
		dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(PerspectiveCameraController::OnMouseMove));
		//dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(PerspectiveCameraController::OnKeyPress));
		//AU_CORE_INFO("Controller onEvent called! {0}", e.GetName());
		//dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(PerspectiveCameraController::OnMouseButtonPress));
		//dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FN(PerspectiveCameraController::OnMouseButtonRelease));
	}

	void PerspectiveCameraController::OnResize(float width, float height) {
		m_TargetCamera->SetLens(0.25f * MathHelper::Pi, AspectRatio(width, height), .1f, 1000.f);
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
				ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;

				double x = 0, y = 0;
				glfwGetCursorPos(window, &x, &y);
				m_LastMousePosition = math::Vec2(static_cast<float>(x), static_cast<float>(y));
				if (glfwRawMouseMotionSupported())
					glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
			} else {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				Application::Get().ImGuiBlockEvents(true);
				ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
			}
		}
		return false;
	}

	bool PerspectiveCameraController::OnMouseMove(MouseMovedEvent& e) {
		//if (m_IsActive) {
		//	float dx = e.GetX() - m_LastMousePosition.x;
		//	float dy = e.GetY() - m_LastMousePosition.y;

		//	m_Yaw += dx * m_CameraRotationSpeed;
		//	m_Pitch += dy * m_CameraRotationSpeed;

		//	m_Pitch = ClampPitch(m_Pitch);

		//	m_TargetCamera->SetRotation(
		//		0.0f,
		//		math::Radians(m_Pitch),
		//		math::Radians(m_Yaw)
		//	);

		//	m_LastMousePosition.x = e.GetX();
		//	m_LastMousePosition.y = e.GetY();
		//}
		return false;
	}

	bool PerspectiveCameraController::OnMouseButtonPress(MouseButtonPressedEvent& e) {
		if (e.GetMouseButton() == Mouse::ButtonRight && m_IsActive) {
			m_IsRightMousePressed = true;
		}
		return false;
	}

	bool PerspectiveCameraController::OnMouseButtonRelease(MouseButtonReleasedEvent& e) {
		if (e.GetMouseButton() == Mouse::ButtonRight && !m_IsActive) {
			m_IsRightMousePressed = false;
		}
		return false;
	}
}
