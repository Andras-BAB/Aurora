#include "SandboxLayer.h"

#include "../../Aurora/Vendor/imgui/imgui.h"
#include "Core/Application.h"
#include "Renderer/RenderCommand.h"

namespace Sandbox {

	SandboxLayer::SandboxLayer() : Layer("AppLayer") {
		m_Shader = std::static_pointer_cast<Aurora::OpenGLShader>(
			Aurora::Shader::Create("Shaders/shaderTest.vert", "Shaders/shaderTest.frag")
			);
		m_Shader->Bind();
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
		m_VAO = std::make_shared<Aurora::OpenGLVertexArray>();
		m_VAO->Bind();
		m_VBO = std::make_shared<Aurora::OpenGLVertexBuffer>(vertices, 24 * sizeof(float));
		m_VBO->Bind();

		m_VBO->SetLayout({
			{ Aurora::ShaderDataType::Float3, "Position" },
			{ Aurora::ShaderDataType::Float3, "Color" }
		});

		m_IBO = std::make_shared<Aurora::OpenGLIndexBuffer>(indices, 6);
		m_IBO->Bind();
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
		Aurora::RenderCommand::DrawIndexed(m_VAO);
	}

	void SandboxLayer::OnImGuiRender() {
		if(m_IsImGuiDemoVisible)
			ImGui::ShowDemoWindow();
	}
}
