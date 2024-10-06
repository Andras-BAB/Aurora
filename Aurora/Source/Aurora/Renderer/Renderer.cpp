#include "aupch.h"

#include "Renderer.h"
#include "RenderCommand.h"

namespace Aurora {
	void Renderer::Init() {
		RenderCommand::Init();
	}

	void Renderer::Shutdown() {
		
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height) {
		RenderCommand::SetViewport(0, 0, width, height);
	}

	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray,
		const glm::mat4& transform) {

		shader->Bind();
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
}
