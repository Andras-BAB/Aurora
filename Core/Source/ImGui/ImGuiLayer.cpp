#include <aupch.h>

#include <ImGui/ImGuiLayer.h>

#include <imgui/imgui.h>

#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx11.h>



namespace Aurora {

	ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer") {
	}

	void ImGuiLayer::OnCreate() {
	}

	void ImGuiLayer::OnDestroy() {
	}

	void ImGuiLayer::OnEvent(Event& e) {
		/*if (m_BlockEvents) {
			ImGuiIO& io = ImGui::GetIO();
			e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			e.Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}*/
	}

	void ImGuiLayer::Begin() {
	}

	void ImGuiLayer::End() {
	}


}