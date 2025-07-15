#include "aupch.h"
#include "VulkanFramebuffer.h"

#include "Aurora/Core/Assert.h"

namespace Aurora {
	void VulkanFramebuffer::Bind() {
	}

	void VulkanFramebuffer::Unbind() {
	}

	void VulkanFramebuffer::Invalidate() {
		// vkDestroyFramebuffer(m_Device, m_Framebuffer, nullptr);
	}

	void VulkanFramebuffer::Resize(uint32_t width, uint32_t height) {
	}

	int VulkanFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y) {
		return 0;
	}

	void VulkanFramebuffer::ClearAttachment(uint32_t attachmentIndex, int value) {
	}

	uint32_t VulkanFramebuffer::GetColorAttachmentRendererID(uint32_t index) const {
		AU_CORE_ASSERT(index < m_ColorAttachments.size());
		return m_ColorAttachments[index];
	}

	const FramebufferSpecification& VulkanFramebuffer::GetSpecification() const {
		return m_Specification;
	}
}
