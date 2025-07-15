#pragma once
#include "Aurora/Renderer/Framebuffer.h"

#include <vector>
#include <vulkan/vulkan.h>

namespace Aurora {
	
	class VulkanFramebuffer : public Framebuffer {
	public:
		void Bind() override;
		void Unbind() override;

		void Invalidate() override;

		void Resize(uint32_t width, uint32_t height) override;
		int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

		void ClearAttachment(uint32_t attachmentIndex, int value) override;

		uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override;

		const FramebufferSpecification& GetSpecification() const override;

	private:
		friend class VulkanSwapChain;
		
		VkFramebuffer m_Framebuffer = VK_NULL_HANDLE;
		
		FramebufferSpecification m_Specification;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};

}
