#pragma once

#include "Aurora.h"
#include "Platform/OpenGL/OpenGLBuffer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"
#include "Renderer/Camera.h"
#include "Renderer/PerspectiveCamera.h"
#include "Renderer/PerspectiveCameraController.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"

namespace Sandbox {

	class SandboxLayer : public Aurora::Layer {
	public:

		SandboxLayer();

		void OnAttach() override;
		void OnDetach() override;
		void OnEvent(Aurora::Event& e) override;
		void OnUpdate(Aurora::Timestep ts) override;
		void OnImGuiRender() override;
		
		bool OnKeyPress(Aurora::KeyPressedEvent& e);
		// bool OnWindowResize(Aurora::WindowResizeEvent& e);

	private:
		bool m_IsImGuiDemoVisible = false;

		std::shared_ptr<Aurora::Framebuffer> m_Framebuffer;
		
		std::shared_ptr<Aurora::Scene> m_Scene;
		Aurora::Entity m_Cube;
		
		std::shared_ptr<Aurora::VertexArray> m_VAO;
		std::shared_ptr<Aurora::VertexBuffer> m_VBO;
		std::shared_ptr<Aurora::IndexBuffer> m_IBO;
		std::shared_ptr<Aurora::Shader> m_Shader;
		// std::shared_ptr<Aurora::OpenGLShader> m_NormalsShader;
		// std::shared_ptr<Aurora::OpenGLShader> m_VectorsShader;
		
		Aurora::PerspectiveCameraController m_CameraController;

		glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 rotation = glm::vec3(0.0f);
		glm::vec3 scale = glm::vec3(1.0f);
		// glm::mat4 model = glm::translate(glm::mat4(1.0), translation) *
		//  glm::rotate(glm::mat4(1.0), 0.0f, rotation) * glm::scale(glm::mat4(1.0), scale);
		glm::mat4 model = glm::translate(glm::mat4(1.0), translation);

		glm::vec4 m_LightPosition = glm::vec4(10.f, 20.f, 4.f, 1.f);
		glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 objectColor = glm::vec3(1.0f, 1.0f, 1.0f);
		float shine = 1.0f;

		std::shared_ptr<Aurora::MeshAsset> m_Mesh;
	};

}
