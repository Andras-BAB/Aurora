#pragma once

#include "Aurora.h"
#include "Platform/OpenGL/OpenGLBuffer.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Sandbox {

	class SandboxLayer : public Aurora::Layer {
	public:

		SandboxLayer();

		void OnAttach() override;
		void OnDetach() override;
		void OnEvent(Aurora::Event& e) override;
		void OnUpdate(Aurora::Timestep ts) override;
		void OnImGuiRender() override;

	private:
		bool m_IsImGuiDemoVisible = false;

		std::shared_ptr<Aurora::OpenGLVertexArray> m_VAO;
		std::shared_ptr<Aurora::OpenGLVertexBuffer> m_VBO;
		std::shared_ptr<Aurora::OpenGLIndexBuffer> m_IBO;
		std::shared_ptr<Aurora::OpenGLShader> m_Shader;
	};

}
