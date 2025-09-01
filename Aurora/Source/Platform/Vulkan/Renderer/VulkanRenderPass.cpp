#include "aupch.h"
#include "VulkanRenderPass.h"

#include "VulkanRenderCommand.h"
#include "VulkanFramebuffer.h"
#include "VulkanContext.h"
#include "Aurora/Renderer/RenderCommand.h"
#include "Aurora/Core/Log.h"
#include "Platform/Vulkan/Utils/VulkanHelper.h"

namespace Aurora {
	VulkanRenderPass::VulkanRenderPass(const RenderPassSpecification& spec) 
		: m_Specification(spec) {
		m_Device = VulkanRenderCommand::GetDevice();
		CreateRenderPass();
	}

	VulkanRenderPass::~VulkanRenderPass() {
		Cleanup();
	}

	void VulkanRenderPass::Begin(const std::shared_ptr<Framebuffer>& framebuffer) {
		m_ActiveFramebuffer = framebuffer;
		m_Width = framebuffer->GetSpecification().Width;
		m_Height = framebuffer->GetSpecification().Height;

		// Get current command buffer from context
		VulkanContext* context = RenderCommand::GetContextAs<VulkanContext>();
		m_ActiveCommandBuffer = context->GetCurrentCommandBuffer();

		// Get Vulkan framebuffer
		auto vulkanFramebuffer = std::static_pointer_cast<VulkanFramebuffer>(framebuffer);

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_RenderPass;
		renderPassInfo.framebuffer = vulkanFramebuffer->GetVulkanFramebuffer();
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = { m_Width, m_Height };

		// Setup clear values
		std::vector<VkClearValue> clearValues;
		for (const auto& attachment : m_Specification.attachments) {
			VkClearValue clearValue{};
			if (attachment.isDepthStencil) {
				clearValue.depthStencil = { 1.0f, 0 };
			} else {
				clearValue.color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
			}
			clearValues.push_back(clearValue);
		}

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(m_ActiveCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanRenderPass::End() {
		if (m_ActiveCommandBuffer != VK_NULL_HANDLE) {
			vkCmdEndRenderPass(m_ActiveCommandBuffer);
			m_ActiveCommandBuffer = VK_NULL_HANDLE;
		}
		m_ActiveFramebuffer = nullptr;
	}

	void VulkanRenderPass::CreateRenderPass() {
		std::vector<VkAttachmentDescription> vkAttachments;
		std::vector<VkAttachmentReference> colorAttachmentRefs;
		VkAttachmentReference depthAttachmentRef{};
		bool hasDepthAttachment = false;

		// Convert Aurora attachments to Vulkan attachments
		for (size_t i = 0; i < m_Specification.attachments.size(); ++i) {
			const auto& attachment = m_Specification.attachments[i];

			VkAttachmentDescription vkAttachment{};
			vkAttachment.format = ToVulkanFormat(attachment.format);
			vkAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
			vkAttachment.loadOp = ToVulkanLoadOp(attachment.loadOp);
			vkAttachment.storeOp = ToVulkanStoreOp(attachment.storeOp);
			vkAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			vkAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			vkAttachment.initialLayout = ToVulkanImageLayout(attachment.initialLayout);
			vkAttachment.finalLayout = ToVulkanImageLayout(attachment.finalLayout);
			vkAttachments.push_back(vkAttachment);

			if (attachment.isDepthStencil) {
				depthAttachmentRef.attachment = static_cast<uint32_t>(i);
				depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				hasDepthAttachment = true;
			} else {
				VkAttachmentReference colorRef{};
				colorRef.attachment = static_cast<uint32_t>(i);
				colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				colorAttachmentRefs.push_back(colorRef);
			}
		}

		// Create subpass descriptions
		std::vector<VkSubpassDescription> vkSubpasses;
		for (const auto& subpassInfo : m_Specification.subpasses) {
			VkSubpassDescription subpass{};
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

			// Color attachments
			if (!colorAttachmentRefs.empty()) {
				subpass.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentRefs.size());
				subpass.pColorAttachments = colorAttachmentRefs.data();
			}

			// Depth attachment
			if (hasDepthAttachment) {
				subpass.pDepthStencilAttachment = &depthAttachmentRef;
			}

			vkSubpasses.push_back(subpass);
		}

		// Create subpass dependencies for external transitions
		std::vector<VkSubpassDependency> dependencies;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies.push_back(dependency);

		if (hasDepthAttachment) {
			VkSubpassDependency depthDependency{};
			depthDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			depthDependency.dstSubpass = 0;
			depthDependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			depthDependency.srcAccessMask = 0;
			depthDependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			depthDependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			dependencies.push_back(depthDependency);
		}

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(vkAttachments.size());
		renderPassInfo.pAttachments = vkAttachments.data();
		renderPassInfo.subpassCount = static_cast<uint32_t>(vkSubpasses.size());
		renderPassInfo.pSubpasses = vkSubpasses.data();
		renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
		renderPassInfo.pDependencies = dependencies.data();

		VK_CHECK(vkCreateRenderPass(m_Device, &renderPassInfo, nullptr, &m_RenderPass));

		AU_CORE_INFO("VulkanRenderPass '{0}' created successfully", m_Specification.name);
	}

	void VulkanRenderPass::Cleanup() {
		if (m_RenderPass != VK_NULL_HANDLE && m_Device != VK_NULL_HANDLE) {
			vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);
			m_RenderPass = VK_NULL_HANDLE;
		}
	}

	VkFormat VulkanRenderPass::ToVulkanFormat(FramebufferTextureFormat format) {
		switch (format) {
		case FramebufferTextureFormat::RGBA8:
			return VK_FORMAT_R8G8B8A8_UNORM;
		case FramebufferTextureFormat::RED_INTEGER:
			return VK_FORMAT_R32_SINT;
		case FramebufferTextureFormat::DEPTH24STENCIL8:
			return VK_FORMAT_D24_UNORM_S8_UINT;
		default:
			AU_CORE_ERROR("Unknown FramebufferTextureFormat!");
			return VK_FORMAT_UNDEFINED;
		}
	}

	VkAttachmentLoadOp VulkanRenderPass::ToVulkanLoadOp(AttachmentLoadOp loadOp) {
		switch (loadOp) {
		case AttachmentLoadOp::Load:
			return VK_ATTACHMENT_LOAD_OP_LOAD;
		case AttachmentLoadOp::Clear:
			return VK_ATTACHMENT_LOAD_OP_CLEAR;
		case AttachmentLoadOp::DontCare:
			return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		default:
			return VK_ATTACHMENT_LOAD_OP_CLEAR;
		}
	}

	VkAttachmentStoreOp VulkanRenderPass::ToVulkanStoreOp(AttachmentStoreOp storeOp) {
		switch (storeOp) {
		case AttachmentStoreOp::Store:
			return VK_ATTACHMENT_STORE_OP_STORE;
		case AttachmentStoreOp::DontCare:
			return VK_ATTACHMENT_STORE_OP_DONT_CARE;
		default:
			return VK_ATTACHMENT_STORE_OP_STORE;
		}
	}

	VkImageLayout VulkanRenderPass::ToVulkanImageLayout(ImageLayout layout) {
		switch (layout) {
		case ImageLayout::Undefined:
			return VK_IMAGE_LAYOUT_UNDEFINED;
		case ImageLayout::ColorAttachment:
			return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		case ImageLayout::DepthStencilAttachment:
			return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		case ImageLayout::ShaderReadOnly:
			return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		case ImageLayout::Present:
			return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		case ImageLayout::General:
			return VK_IMAGE_LAYOUT_GENERAL;
		default:
			return VK_IMAGE_LAYOUT_UNDEFINED;
		}
	}
}
