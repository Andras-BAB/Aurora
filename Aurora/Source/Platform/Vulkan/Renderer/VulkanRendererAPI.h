#pragma once
#include "VulkanContext.h"
#include "Aurora/Renderer/RendererAPI.h"

namespace Aurora {
	class VulkanRendererAPI : public RendererAPI {
	public:
		virtual void Init() override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual void SetScissors(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual void ResizeSwapchainImage(uint32_t width, uint32_t height) override;

		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;

		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
		virtual void DrawLines(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount) override;

		void SetLineWidth(float width) override;

		void SetContext(GraphicsContext* context) override;
		GraphicsContext* GetContext() const override;
		
	private:
		VulkanContext* m_Context = nullptr;

		VkCommandPool m_CommandPool = VK_NULL_HANDLE;
		std::vector<VkCommandBuffer> m_CommandBuffers;
	};
}
