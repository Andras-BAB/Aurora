#include "aupch.h"

#include "UIHelper.h"
#include "Aurora/Core/Application.h"

#include <imgui.h>
#include <GLFW/glfw3.h>

namespace Aurora::UI {
	bool InfiniteDragFloat(const char* label, float* v, float v_speed, float v_min, float v_max, const char* format) {
		bool changed = ImGui::DragFloat(label, v, v_speed, v_min, v_max, format);

		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());

		if (ImGui::IsItemActive()) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		} else if (ImGui::IsItemDeactivated()) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

		return changed;
	}

	bool InfiniteDragFloat3(const char* label, float* v, float v_speed, float v_min, float v_max, const char* format) {
		bool changed = ImGui::DragFloat3(label, v, v_speed, v_min, v_max, format);

		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());

		if (ImGui::IsItemActive()) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		} else if (ImGui::IsItemDeactivated()) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

		return changed;
	}
}
