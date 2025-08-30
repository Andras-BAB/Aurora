#pragma once

#include <vulkan/vulkan.h>

namespace Aurora {
	struct AttachmentDescription {
		VkFormat format = VK_FORMAT_UNDEFINED;
		VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
		VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		VkAttachmentLoadOp stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		VkAttachmentStoreOp stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		VkImageLayout initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		VkImageLayout finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	};

	struct SubpassDescription {
		std::vector<VkAttachmentReference> inputAttachments;
		std::vector<VkAttachmentReference> colorAttachments;
		std::vector<VkAttachmentReference> resolveAttachments; // MSAA resolve
		VkAttachmentReference depthStencilAttachment = { VK_ATTACHMENT_UNUSED, VK_IMAGE_LAYOUT_UNDEFINED };
		std::vector<uint32_t> preserveAttachments;
	};

	struct RenderPassConfig {
		std::vector<AttachmentDescription> attachments;
		std::vector<SubpassDescription> subpasses;
		std::vector<VkSubpassDependency> dependencies;
	};
	
	class VulkanRenderPass {
	public:
		VulkanRenderPass(const RenderPassConfig& imageFormat);
		virtual ~VulkanRenderPass() = default;

		VkRenderPass GetHandle() const { return m_RenderPass; }

	private:
		void CreateRenderPass();
		
	private:
		VkDevice m_Device;
		VkRenderPass m_RenderPass = VK_NULL_HANDLE;
		RenderPassConfig m_Config;
	};
}
