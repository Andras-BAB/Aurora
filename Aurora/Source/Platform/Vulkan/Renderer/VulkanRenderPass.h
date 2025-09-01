#pragma once

#include "Aurora/Renderer/RenderPass.h"
#include <vulkan/vulkan.h>

namespace Aurora {

	class VulkanRenderPass : public RenderPass {
	public:
		VulkanRenderPass(const RenderPassSpecification& spec);
		virtual ~VulkanRenderPass();

		void Begin(const std::shared_ptr<Framebuffer>& framebuffer) override;
		void End() override;

		const RenderPassSpecification& GetSpecification() const override { return m_Specification; }
		uint32_t GetWidth() const override { return m_Width; }
		uint32_t GetHeight() const override { return m_Height; }

		VkRenderPass GetVulkanRenderPass() const { return m_RenderPass; }

	private:
		void CreateRenderPass();
		void Cleanup();

		VkFormat ToVulkanFormat(FramebufferTextureFormat format);
		VkAttachmentLoadOp ToVulkanLoadOp(AttachmentLoadOp loadOp);
		VkAttachmentStoreOp ToVulkanStoreOp(AttachmentStoreOp storeOp);
		VkImageLayout ToVulkanImageLayout(ImageLayout layout);

	private:
		RenderPassSpecification m_Specification;
		VkDevice m_Device = VK_NULL_HANDLE;
		VkRenderPass m_RenderPass = VK_NULL_HANDLE;

		uint32_t m_Width = 0;
		uint32_t m_Height = 0;

		// Active rendering state
		VkCommandBuffer m_ActiveCommandBuffer = VK_NULL_HANDLE;
		std::shared_ptr<Framebuffer> m_ActiveFramebuffer;
	};

}
