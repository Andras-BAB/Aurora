#include "aupch.h"

#include "Aurora/Core/Application.h"
#include "Aurora/Core/Assert.h"
#include "Platform/Vulkan/Renderer/VulkanRendererAPI.h"

#include <cstdlib>

namespace Aurora {
	void VulkanRendererAPI::Init() {
		
	}

	void VulkanRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
		
	}

	void VulkanRendererAPI::SetScissors(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
	}

	void VulkanRendererAPI::ResizeSwapchainImage(uint32_t width, uint32_t height) {
	}

	void VulkanRendererAPI::SetClearColor(const glm::vec4& color) {
	}

	void VulkanRendererAPI::Clear() {
	}

	// void VulkanRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount) {
	// }
	//
	// void VulkanRendererAPI::DrawLines(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount) {
	// }

	void VulkanRendererAPI::DrawIndexed(const std::shared_ptr<MeshAsset>& meshAsset) {		
		VkCommandBuffer cmdBuffer = m_Context->GetCurrentCommandBuffer();
		
		meshAsset->BindBuffers();

		vkCmdDrawIndexed(cmdBuffer, meshAsset->m_IndexBuffer->GetCount(), 1, 0, 0, 0);
	}

	void VulkanRendererAPI::SetLineWidth(float width) {
	}

	void VulkanRendererAPI::SetContext(GraphicsContext* context) {
		m_Context = dynamic_cast<VulkanContext*>(context);
	}

	GraphicsContext* VulkanRendererAPI::GetContext() const {
		AU_CORE_ASSERT(m_Context, "Graphics context is null!")
		return m_Context;
	}
}
