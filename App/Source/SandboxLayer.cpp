#include "SandboxLayer.h"

#include "../../Aurora/Vendor/imgui/imgui.h"
#include "Core/Application.h"
#include "Renderer/RenderCommand.h"

namespace Sandbox {

	SandboxLayer::SandboxLayer() : Layer("AppLayer") {
	}

	void SandboxLayer::OnAttach() {
	
	}

	void SandboxLayer::OnDetach() {
		
	}

	void SandboxLayer::OnEvent(Aurora::Event& e) {

	}

	void SandboxLayer::OnUpdate(Aurora::Timestep ts) {
		Aurora::RenderCommand::SetClearColor({ 0.5f, 0.5f, 0.5f, 1.0f });
		Aurora::RenderCommand::Clear();
	}

	void SandboxLayer::OnImGuiRender() {
		ImGui::ShowDemoWindow(&m_IsImGuiDemoVisible);
	}
}
