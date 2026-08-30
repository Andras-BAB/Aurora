#include "aupch.h"
#include "Aurora/Core/Input.h"
#include "Aurora/Core/KeyCodes.h"

#include "Aurora/Core/Application.h"
#include <GLFW/glfw3.h>

namespace Aurora {

	bool Input::IsKeyPressed(const KeyCode key, void* targetWindow) {
		GLFWwindow* window = targetWindow ?
			static_cast<GLFWwindow*>(targetWindow) :
			static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, static_cast<int32_t>(key));
		return state == GLFW_PRESS;
	}

	bool Input::IsMouseButtonPressed(const MouseCode button, void* targetWindow) {
		GLFWwindow* window = targetWindow ?
			static_cast<GLFWwindow*>(targetWindow) :
			static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}

	math::Vec2 Input::GetMousePosition(void* targetWindow) {
		GLFWwindow* window = targetWindow ?
			static_cast<GLFWwindow*>(targetWindow) :
			static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float) xpos, (float) ypos };
	}

	float Input::GetMouseX() {
		return GetMousePosition().x;
	}

	float Input::GetMouseY() {
		return GetMousePosition().y;
	}

}