#include "SandboxLayer.h"

#include "../../Aurora/Vendor/imgui/imgui.h"
#include "Core/Application.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer3D.h"

namespace Sandbox {

	SandboxLayer::SandboxLayer() : Layer("AppLayer") {
		m_Shader = Aurora::Shader::Create("Shaders/shaderTest.vert", "Shaders/shaderTest.frag");

		float vertices[] = {
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
			0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			-0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f,

			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
			0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
			-0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
		};
		unsigned int indices[] = {
			// Front
			0, 1, 2,
			0, 2, 3,
			// Back
			4, 6, 5,
			4, 7, 6,
			// Top
			3, 2, 6,
			3, 6, 7,
			// Bottom
			0, 5, 1,
			0, 4, 5,
			// Right
			1, 5, 6,
			1, 6, 2,
			// Left
			0, 7, 4,
			0, 3, 7
		};
		m_VAO = Aurora::VertexArray::Create();
		m_VBO = Aurora::VertexBuffer::Create(vertices, sizeof(vertices));

		m_VBO->SetLayout({
			{ Aurora::ShaderDataType::Float3, "Position" },
			{ Aurora::ShaderDataType::Float3, "Color" }
		});

		m_IBO = Aurora::IndexBuffer::Create(indices, std::size(indices));
		m_VAO->AddVertexBuffer(m_VBO);
		m_VAO->SetIndexBuffer(m_IBO);
	}

	void SandboxLayer::OnAttach() {
		Aurora::RenderCommand::SetClearColor({ 0.5f, 0.5f, 0.5f, 1.0f });
	}

	void SandboxLayer::OnDetach() {
		
	}

	void SandboxLayer::OnEvent(Aurora::Event& e) {
		m_CameraController.OnEvent(e);
		
		Aurora::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Aurora::KeyPressedEvent>(BIND_EVENT_FN(SandboxLayer::OnKeyPress));
		//dispatcher.Dispatch<Aurora::WindowResizeEvent>(BIND_EVENT_FN(SandboxLayer::OnWindowResize));
	}

	void SandboxLayer::OnUpdate(Aurora::Timestep ts) {
		m_CameraController.OnUpdate(ts);

		Aurora::RenderCommand::Clear();
		
		std::dynamic_pointer_cast<Aurora::OpenGLShader>(m_Shader)->SetMat4("u_Model",
			glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));

		std::dynamic_pointer_cast<Aurora::OpenGLShader>(m_Shader)->SetMat4("u_ViewProjection",
					m_CameraController.GetCamera().GetViewProjectionMatrix());
		
		// Aurora::Renderer3D::Draw(m_VAO);
		Aurora::RenderCommand::DrawIndexed(m_VAO);
	}

	void SandboxLayer::OnImGuiRender() {
		if(m_IsImGuiDemoVisible){
			ImGui::ShowDemoWindow();
		}

		ImGui::Begin("Infos");
		ImGui::Text("Position:");
		ImGui::Text("\tX: %.2f", m_CameraController.GetCameraPosition().x);
		ImGui::Text("\tY: %.2f", m_CameraController.GetCameraPosition().y);
		ImGui::Text("\tZ: %.2f", m_CameraController.GetCameraPosition().z);
		ImGui::Text("Rotation:");
		ImGui::Text("\tPitch: %.2f", m_CameraController.GetCameraRotation().y);
		ImGui::Text("\tYaw: %.2f", m_CameraController.GetCameraRotation().z);
		ImGui::End();
	}

	bool SandboxLayer::OnKeyPress(Aurora::KeyPressedEvent& e) {
		if(e.GetKeyCode() == Aurora::Key::I) {
			m_IsImGuiDemoVisible = !m_IsImGuiDemoVisible;
		}
		return false;
	}

	// bool SandboxLayer::OnWindowResize(Aurora::WindowResizeEvent& e) {
	// 	return false;
	// }
}
