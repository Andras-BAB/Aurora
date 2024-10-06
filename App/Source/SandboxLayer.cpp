#include "SandboxLayer.h"

#include "../../Aurora/Vendor/imgui/imgui.h"
#include "Core/Application.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer3D.h"

namespace Sandbox {

	SandboxLayer::SandboxLayer() : Layer("AppLayer") {
		m_Shader = Aurora::Shader::Create("Shaders/shaderTest.vert", "Shaders/shaderTest.frag");
		
		float vertices[] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
			0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
			-0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f,
		};
		unsigned int indices[] = {
			0, 1, 2,
			0, 2, 3
		};
		m_VAO = Aurora::VertexArray::Create();
		m_VBO = Aurora::VertexBuffer::Create(vertices, 24 * sizeof(float));

		m_VBO->SetLayout({
			{ Aurora::ShaderDataType::Float3, "Position" },
			{ Aurora::ShaderDataType::Float3, "Color" }
		});

		m_IBO = Aurora::IndexBuffer::Create(indices, 6);
		m_VAO->AddVertexBuffer(m_VBO);
		m_VAO->SetIndexBuffer(m_IBO);
	}

	void SandboxLayer::OnAttach() {
		Aurora::RenderCommand::SetClearColor({ 0.5f, 0.5f, 0.5f, 1.0f });
	}

	void SandboxLayer::OnDetach() {
		
	}

	void SandboxLayer::OnEvent(Aurora::Event& e) {

	}

	void SandboxLayer::OnUpdate(Aurora::Timestep ts) {
		Aurora::RenderCommand::Clear();
		//Aurora::RenderCommand::DrawIndexed(m_VAO);
		Aurora::Renderer3D::Draw(m_VAO);
	}

	void SandboxLayer::OnImGuiRender() {
		if(m_IsImGuiDemoVisible)
			ImGui::ShowDemoWindow();
	}
}
