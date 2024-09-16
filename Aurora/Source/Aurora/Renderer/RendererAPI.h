#pragma once
#include <glm/glm.hpp>

#include "VertexArray.h"

namespace Aurora {
	class RendererAPI {
	public:
		enum class API {
			None = 0,
			Vulkan = 1,
			OpenGL = 2,
			DirectX12 = 3,
			DirectX11 = 4
		};
	public:
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void SetScissors(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual void ResizeSwapchainImage(uint32_t width, uint32_t height) = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
		virtual void DrawLines(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount) = 0;

		virtual void SetLineWidth(float width) = 0;

		static API GetAPI() { return s_API; }
		static std::unique_ptr<RendererAPI> Create();

	private:
		static API s_API;
	};
}
