#include "aupch.h"
#include "VulkanFramebuffer.h"

#include "VulkanRenderCommand.h"
#include "Aurora/Core/Assert.h"

namespace Aurora {
	VulkanFramebuffer::VulkanFramebuffer(const FramebufferSpecification& spec) : m_Specification(spec) {
	}

	void VulkanFramebuffer::Bind() {
	}

	void VulkanFramebuffer::Unbind() {
	}

	void VulkanFramebuffer::Invalidate() {
		// TODO: finish the rest
		VkDevice device = VulkanRenderCommand::GetDevice();
		
		if (m_Framebuffer != VK_NULL_HANDLE) {
			vkDestroyFramebuffer(device, m_Framebuffer, nullptr);
			m_Framebuffer = VK_NULL_HANDLE;
			// Itt a képek és view-k törlése is kell
		}

		// Attachmentek (color + depth) létrehozása a m_Specification alapján
		std::vector<VkImageView> attachments;

		for (auto& spec : m_ColorAttachmentSpecifications) {
			// VkImage, VkImageView létrehozása (usage: COLOR_ATTACHMENT_BIT)
			// attachments.push_back(imageView);
		}

		if (m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None) {
			// Depth VkImage, VkImageView létrehozása (usage: DEPTH_STENCIL_ATTACHMENT_BIT)
			// attachments.push_back(depthImageView);
		}

		VkFramebufferCreateInfo fbInfo{};
		fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		// fbInfo.renderPass = /* renderPassHandle */;
		fbInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		fbInfo.pAttachments = attachments.data();
		fbInfo.width = m_Specification.Width;
		fbInfo.height = m_Specification.Height;
		fbInfo.layers = 1;

		if (vkCreateFramebuffer(device, &fbInfo, nullptr, &m_Framebuffer) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Vulkan framebuffer!");
		}
	}

	void VulkanFramebuffer::Resize(uint32_t width, uint32_t height) {
		m_Specification.Width = width;
		m_Specification.Height = height;
		Invalidate();
	}

	int VulkanFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y) {
		return 0;
	}

	void VulkanFramebuffer::ClearAttachment(uint32_t attachmentIndex, int value) {
		VkClearAttachment clearAttachment{};
		clearAttachment.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		clearAttachment.colorAttachment = attachmentIndex;
		clearAttachment.clearValue.color = { { (float)value, 0.0f, 0.0f, 1.0f } }; // formátumtól függ

		VkClearRect clearRect{};
		clearRect.rect.offset = { 0, 0 };
		clearRect.rect.extent = { m_Specification.Width, m_Specification.Height };
		clearRect.baseArrayLayer = 0;
		clearRect.layerCount = 1;

		// vkCmdClearAttachments(cmdBuffer, 1, &clearAttachment, 1, &clearRect);
	}

	uint32_t VulkanFramebuffer::GetColorAttachmentRendererID(uint32_t index) const {
		AU_CORE_ASSERT(index < m_ColorAttachments.size());
		return m_ColorAttachments[index];
		// return 0;
	}

	const FramebufferSpecification& VulkanFramebuffer::GetSpecification() const {
		return m_Specification;
	}
}
