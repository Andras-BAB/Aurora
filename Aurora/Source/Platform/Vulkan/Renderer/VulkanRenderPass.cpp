#include "aupch.h"
#include "VulkanRenderPass.h"

#include "VulkanRenderCommand.h"
#include "Platform/Vulkan/Utils/VulkanHelper.h"

namespace Aurora {
	VulkanRenderPass::VulkanRenderPass(const RenderPassConfig& imageFormat) : m_Config(imageFormat) {
		m_Device = VulkanRenderCommand::GetDevice();
		CreateRenderPass();
	}

	void VulkanRenderPass::CreateRenderPass() {
		std::vector<VkAttachmentDescription> vkAttachments;
	    for (const auto& attachment : m_Config.attachments) {
	        VkAttachmentDescription vkAttachment{};
	        vkAttachment.format = attachment.format;
	        vkAttachment.samples = attachment.samples;
	        vkAttachment.loadOp = attachment.loadOp;
	        vkAttachment.storeOp = attachment.storeOp;
	        vkAttachment.stencilLoadOp = attachment.stencilLoadOp;
	        vkAttachment.stencilStoreOp = attachment.stencilStoreOp;
	        vkAttachment.initialLayout = attachment.initialLayout;
	        vkAttachment.finalLayout = attachment.finalLayout;
	        vkAttachments.push_back(vkAttachment);
	    }

	    std::vector<VkSubpassDescription> vkSubpasses;
	    for (const auto& subpass : m_Config.subpasses) {
	        VkSubpassDescription vkSubpass{};
	        vkSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	        vkSubpass.inputAttachmentCount = static_cast<uint32_t>(subpass.inputAttachments.size());
	        vkSubpass.pInputAttachments = subpass.inputAttachments.data();
	        vkSubpass.colorAttachmentCount = static_cast<uint32_t>(subpass.colorAttachments.size());
	        vkSubpass.pColorAttachments = subpass.colorAttachments.data();
	        vkSubpass.pResolveAttachments = subpass.resolveAttachments.empty() ? nullptr : subpass.resolveAttachments.data();
	        vkSubpass.pDepthStencilAttachment = (subpass.depthStencilAttachment.attachment == VK_ATTACHMENT_UNUSED) ? nullptr : &subpass.depthStencilAttachment;
	        vkSubpass.preserveAttachmentCount = static_cast<uint32_t>(subpass.preserveAttachments.size());
	        vkSubpass.pPreserveAttachments = subpass.preserveAttachments.data();
	        vkSubpasses.push_back(vkSubpass);
	    }

	    VkRenderPassCreateInfo renderPassInfo{};
	    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	    renderPassInfo.attachmentCount = static_cast<uint32_t>(vkAttachments.size());
	    renderPassInfo.pAttachments = vkAttachments.data();
	    renderPassInfo.subpassCount = static_cast<uint32_t>(vkSubpasses.size());
	    renderPassInfo.pSubpasses = vkSubpasses.data();
	    renderPassInfo.dependencyCount = static_cast<uint32_t>(m_Config.dependencies.size());
	    renderPassInfo.pDependencies = m_Config.dependencies.data();

	    VK_CHECK(vkCreateRenderPass(m_Device, &renderPassInfo, nullptr, &m_RenderPass));
	}
}
