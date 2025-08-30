#pragma once
#include "VulkanContext.h"
#include "Aurora/Renderer/RendererAPI.h"

namespace Aurora {
	class VulkanRendererAPI : public RendererAPI {
	public:
		void Init() override;
		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		void SetScissors(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		void ResizeSwapchainImage(uint32_t width, uint32_t height) override;

		void SetClearColor(const glm::vec4& color) override;
		void Clear() override;

		// virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
		// virtual void DrawLines(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount) override;

		void DrawIndexed(const std::shared_ptr<MeshAsset>& meshAsset) override;

		void SetLineWidth(float width) override;

		void SetContext(GraphicsContext* context) override;
		GraphicsContext* GetContext() const override;
		
	private:
		friend class MeshAsset;
		
		VulkanContext* m_Context = nullptr;

		VkCommandPool m_CommandPool = VK_NULL_HANDLE;
		std::vector<VkCommandBuffer> m_CommandBuffers;
	};
}
