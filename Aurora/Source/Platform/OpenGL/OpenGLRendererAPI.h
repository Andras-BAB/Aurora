#pragma once
#include "Aurora/Renderer/RendererAPI.h"

namespace Aurora {
	class OpenGLRendererAPI : public RendererAPI {
	public:
		OpenGLRendererAPI() = default;
		~OpenGLRendererAPI() override = default;

		void Init() override;
		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		void SetScissors(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		void ResizeSwapchainImage(uint32_t width, uint32_t height) override;
		void SetClearColor(const glm::vec4& color) override;
		void Clear() override;

		void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount) override;
		void DrawLines(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount) override;

		void SetLineWidth(float width) override;
	};
}