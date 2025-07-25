#include "aupch.h"
#include "VulkanRenderCommand.h"

#include "VulkanBuffer.h"
#include "VulkanContext.h"
#include "Aurora/Renderer/RenderCommand.h"

namespace Aurora {
	void VulkanRenderCommand::BeginScene() {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		VulkanContext* context = RenderCommand::GetContextAs<VulkanContext>();
		VkCommandBuffer cmdBuffer = context->GetCurrentCommandBuffer();

		if (vkBeginCommandBuffer(cmdBuffer, &beginInfo) != VK_SUCCESS) {
			AU_CORE_ERROR("Failed to begin recording command buffer!");
		}

		VulkanSwapChain swapChain = context->GetSwapChain();
		uint32_t imageIndex = context->GetCurrentFrame();
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = swapChain.GetRenderPass();
		renderPassInfo.framebuffer = swapChain.GetFrameBuffers()[imageIndex];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapChain.GetExtent();

		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, swapChain.GetPipeline().GetGraphicsPipeline());

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float) swapChain.GetExtent().width;
		viewport.height = (float) swapChain.GetExtent().height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = swapChain.GetExtent();
		vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);
	}

	void VulkanRenderCommand::EndScene() {
		VulkanContext* context = RenderCommand::GetContextAs<VulkanContext>();
		VkCommandBuffer cmdBuffer = context->GetCurrentCommandBuffer();
		vkCmdEndRenderPass(cmdBuffer);
		
		if (vkEndCommandBuffer(cmdBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}
