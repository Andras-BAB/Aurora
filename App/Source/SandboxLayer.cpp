#include "SandboxLayer.h"

#include "../../Aurora/Vendor/imgui/imgui.h"
#include "Core/Application.h"
#include "Platform/Vulkan/Renderer/VulkanRenderCommand.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer3D.h"

namespace Sandbox {

	SandboxLayer::SandboxLayer() : Layer("AppLayer") {
		return;
		
		m_Shader = Aurora::Shader::Create("Shaders/phong.vert", "Shaders/phong.frag");
		m_Scene = std::make_shared<Aurora::Scene>();
		// m_NormalsShader = std::make_shared<Aurora::OpenGLShader>("Shaders/normal.vert", "Shaders/normal.frag", "Shaders/normal.geom");
		
		// m_VectorsShader = std::make_shared<Aurora::OpenGLShader>("Shaders/vectors.vert", "Shaders/vectors.frag", "Shaders/vectors.geom");

		Aurora::FramebufferSpecification fbSpec;
		fbSpec.Attachments = {
			Aurora::FramebufferTextureFormat::RGBA8,
			Aurora::FramebufferTextureFormat::RED_INTEGER,
			Aurora::FramebufferTextureFormat::Depth };
		fbSpec.Width = 1600;
		fbSpec.Height = 900;
		//m_Framebuffer = Aurora::Framebuffer::Create(fbSpec);
		
		m_Cube = m_Scene->CreateEntity("Cube");
		//m_Cube.AddComponent<Aurora::MeshComponent>();
		
		float vertices[24 * 6] = {
			// Fill in the front face vertex data.
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
			-0.5f, +0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
			+0.5f, +0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
			+0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

			// Fill in the back face vertex data.
			-0.5f, -0.5f, +0.5f, 0.0f, 0.0f, 1.0f,
			+0.5f, -0.5f, +0.5f, 0.0f, 0.0f, 1.0f,
			+0.5f, +0.5f, +0.5f, 0.0f, 0.0f, 1.0f,
			-0.5f, +0.5f, +0.5f, 0.0f, 0.0f, 1.0f,

			// Fill in the top face vertex data.
			-0.5f, +0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			-0.5f, +0.5f, +0.5f, 0.0f, 1.0f, 0.0f,
			+0.5f, +0.5f, +0.5f, 0.0f, 1.0f, 0.0f,
			+0.5f, +0.5f, -0.5f, 0.0f, 1.0f, 0.0f,

			// Fill in the bottom face vertex data.
			-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
			+0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
			+0.5f, -0.5f, +0.5f, 0.0f, -1.0f, 0.0f,
			-0.5f, -0.5f, +0.5f, 0.0f, -1.0f, 0.0f,

			// Fill in the left face vertex data.
			-0.5f, -0.5f, +0.5f, -1.0f, 0.0f, 0.0f,
			-0.5f, +0.5f, +0.5f, -1.0f, 0.0f, 0.0f,
			-0.5f, +0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,

			// Fill in the right face vertex data.
			+0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
			+0.5f, +0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
			+0.5f, +0.5f, +0.5f, 1.0f, 0.0f, 0.0f,
			+0.5f, -0.5f, +0.5f, 1.0f, 0.0f, 0.0f,
		};

		unsigned int indices[36];
		// Fill in the front face index data
		indices[0] = 0; indices[1] = 2; indices[2] = 1;
		indices[3] = 0; indices[4] = 3; indices[5] = 2;

		// Fill in the back face index data
		indices[6] = 4; indices[7] = 6; indices[8] = 5;
		indices[9] = 4; indices[10] = 7; indices[11] = 6;

		// Fill in the top face index data
		indices[12] = 8; indices[13] = 10; indices[14] = 9;
		indices[15] = 8; indices[16] = 11; indices[17] = 10;

		// Fill in the bottom face index data
		indices[18] = 12; indices[19] = 14; indices[20] = 13;
		indices[21] = 12; indices[22] = 15; indices[23] = 14;

		// Fill in the left face index data
		indices[24] = 16; indices[25] = 18; indices[26] = 17;
		indices[27] = 16; indices[28] = 19; indices[29] = 18;

		// Fill in the right face index data
		indices[30] = 20; indices[31] = 22; indices[32] = 21;
		indices[33] = 20; indices[34] = 23; indices[35] = 22;
		
		// m_VAO = Aurora::VertexArray::Create();
		m_VBO = Aurora::VertexBuffer::Create(vertices, sizeof(vertices));
		
		m_VBO->SetLayout({
			{ Aurora::ShaderDataType::Float3, "Position" },
			{ Aurora::ShaderDataType::Float3, "Normal" }
		});

		m_IBO = Aurora::IndexBuffer::Create(indices, std::size(indices));
		// m_VAO->AddVertexBuffer(m_VBO);
		// m_VAO->SetIndexBuffer(m_IBO);
	}

	void SandboxLayer::OnAttach() {
		std::vector<float> vertices_f = {
			// Fill in the front face vertex data.
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
			-0.5f, +0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
			+0.5f, +0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
			+0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

			// Fill in the back face vertex data.
			-0.5f, -0.5f, +0.5f, 0.0f, 0.0f, 1.0f,
			+0.5f, -0.5f, +0.5f, 0.0f, 0.0f, 1.0f,
			+0.5f, +0.5f, +0.5f, 0.0f, 0.0f, 1.0f,
			-0.5f, +0.5f, +0.5f, 0.0f, 0.0f, 1.0f,

			// Fill in the top face vertex data.
			-0.5f, +0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			-0.5f, +0.5f, +0.5f, 0.0f, 1.0f, 0.0f,
			+0.5f, +0.5f, +0.5f, 0.0f, 1.0f, 0.0f,
			+0.5f, +0.5f, -0.5f, 0.0f, 1.0f, 0.0f,

			// Fill in the bottom face vertex data.
			-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
			+0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
			+0.5f, -0.5f, +0.5f, 0.0f, -1.0f, 0.0f,
			-0.5f, -0.5f, +0.5f, 0.0f, -1.0f, 0.0f,

			// Fill in the left face vertex data.
			-0.5f, -0.5f, +0.5f, -1.0f, 0.0f, 0.0f,
			-0.5f, +0.5f, +0.5f, -1.0f, 0.0f, 0.0f,
			-0.5f, +0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,

			// Fill in the right face vertex data.
			+0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
			+0.5f, +0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
			+0.5f, +0.5f, +0.5f, 1.0f, 0.0f, 0.0f,
			+0.5f, -0.5f, +0.5f, 1.0f, 0.0f, 0.0f,
		};

		std::vector<uint32_t> indices;
		indices.resize(36);
		// Fill in the front face index data
		indices[0] = 0; indices[1] = 2; indices[2] = 1;
		indices[3] = 0; indices[4] = 3; indices[5] = 2;

		// Fill in the back face index data
		indices[6] = 4; indices[7] = 6; indices[8] = 5;
		indices[9] = 4; indices[10] = 7; indices[11] = 6;

		// Fill in the top face index data
		indices[12] = 8; indices[13] = 10; indices[14] = 9;
		indices[15] = 8; indices[16] = 11; indices[17] = 10;

		// Fill in the bottom face index data
		indices[18] = 12; indices[19] = 14; indices[20] = 13;
		indices[21] = 12; indices[22] = 15; indices[23] = 14;

		// Fill in the left face index data
		indices[24] = 16; indices[25] = 18; indices[26] = 17;
		indices[27] = 16; indices[28] = 19; indices[29] = 18;

		// Fill in the right face index data
		indices[30] = 20; indices[31] = 22; indices[32] = 21;
		indices[33] = 20; indices[34] = 23; indices[35] = 22;
		
		std::shared_ptr<Aurora::VertexBuffer> vb = Aurora::VertexBuffer::Create(vertices_f.data(), vertices_f.size());
		vb->SetLayout({
			{ Aurora::ShaderDataType::Float3, "Position" },
			{ Aurora::ShaderDataType::Float3, "Normal" }
		});
		std::shared_ptr<Aurora::IndexBuffer> ib = Aurora::IndexBuffer::Create(indices.data(), std::size(indices));
		m_Mesh = std::make_shared<Aurora::MeshAsset>(vb, ib);
		Aurora::RenderCommand::SetClearColor({ 0.3f, 0.3f, 0.3f, 1.0f });
	}

	void SandboxLayer::OnDetach() {
		m_Mesh.reset();
	}

	void SandboxLayer::OnEvent(Aurora::Event& e) {
		m_CameraController.OnEvent(e);

		Aurora::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Aurora::KeyPressedEvent>(BIND_EVENT_FN(SandboxLayer::OnKeyPress));
	}

	void SandboxLayer::OnUpdate(Aurora::Timestep ts) {
		// Aurora::VulkanRenderCommand::BeginScene();
		//
		// Aurora::RenderCommand::DrawIndexed(m_Mesh);
		//
		// Aurora::VulkanRenderCommand::EndScene();
		
		return;
		m_CameraController.OnUpdate(ts);
		Aurora::RenderCommand::Clear();
		//glPolygonMode(GL_FRONT, GL_LINE);
		//glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);
		
		model = glm::translate(glm::mat4(1.0), translation);

		// m_Shader->Bind();
		std::dynamic_pointer_cast<Aurora::OpenGLShader>(m_Shader)->SetMat4("u_Model",
			model);
			//glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
		
		std::dynamic_pointer_cast<Aurora::OpenGLShader>(m_Shader)->SetMat4("u_ViewProjection",
			m_CameraController.GetCamera().GetViewProjectionMatrix());
		
		// Fragment shader uniforms
		std::dynamic_pointer_cast<Aurora::OpenGLShader>(m_Shader)->SetFloat3("lightPos",
			m_LightPosition);
		
		std::dynamic_pointer_cast<Aurora::OpenGLShader>(m_Shader)->SetFloat3("viewPos",
			m_CameraController.GetCamera().GetPosition());
		
		std::dynamic_pointer_cast<Aurora::OpenGLShader>(m_Shader)->SetFloat3("lightColor",
			lightColor);
		std::dynamic_pointer_cast<Aurora::OpenGLShader>(m_Shader)->SetFloat3("objectColor",
			objectColor);
		
		// Aurora::RenderCommand::DrawIndexed(m_VAO);
		
		// m_NormalsShader->Bind();
		// m_NormalsShader->SetMat4("u_Model", model);
		// m_NormalsShader->SetMat4("u_View", m_CameraController.GetCamera().GetViewMatrix());
		// m_NormalsShader->SetMat4("u_Projection", m_CameraController.GetCamera().GetProjectionMatrix());
		// Aurora::RenderCommand::DrawIndexed(m_VAO);
		
		// m_VectorsShader->Bind();
		// m_VectorsShader->SetMat4("u_Model", model);
		// m_VectorsShader->SetMat4("u_View", m_CameraController.GetCamera().GetViewMatrix());
		// m_VectorsShader->SetMat4("u_Projection", m_CameraController.GetCamera().GetProjectionMatrix());
		// m_VectorsShader->SetFloat3("lightPos", m_LightPosition);
		// m_VectorsShader->SetFloat3("viewPos", m_CameraController.GetCamera().GetPosition());
		// Aurora::RenderCommand::DrawIndexed(m_VAO);
		
	}

	void SandboxLayer::OnImGuiRender() {
		return;
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
		ImGui::SliderFloat3("Light position", &m_LightPosition.x, -20.0f, 20.0f, "%.0f");

		//ImGui::SliderFloat("Shine", &shine, 0.0f, 100.0f, "%.2f");
		//ImGui::SliderFloat("AttenuationX", &attenuation.x, 0.05f, 10.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
		//ImGui::SliderFloat("AttenuationY", &attenuation.y, 0.0001f, 4.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
		//ImGui::SliderFloat("AttenuationZ", &attenuation.z, 0.0000001f, 1.0f, "%.7f", ImGuiSliderFlags_Logarithmic);
		ImGui::ColorEdit3("Light color", &lightColor.x);
		ImGui::ColorEdit3("Object color", &objectColor.x);
		ImGui::SliderFloat3("Model ", &translation.x, -20.0f, 20.0f, "%.2f");
		ImGui::End();
	}

	bool SandboxLayer::OnKeyPress(Aurora::KeyPressedEvent& e) {
		if(e.GetKeyCode() == Aurora::Key::I) {
			m_IsImGuiDemoVisible = !m_IsImGuiDemoVisible;
		}
		return false;
	}
	
}
