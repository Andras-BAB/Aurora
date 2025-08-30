#pragma once
#include "Aurora/Renderer/Framebuffer.h"

#include <vector>
#include <vulkan/vulkan.h>

namespace Aurora {
	
	class VulkanFramebuffer : public Framebuffer {
	public:
		VulkanFramebuffer(const FramebufferSpecification& spec);
		
		void Bind() override;
		void Unbind() override;

		void Invalidate() override;

		void Resize(uint32_t width, uint32_t height) override;
		[[deprecated]] int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

		void ClearAttachment(uint32_t attachmentIndex, int value) override;

		uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override;

		const FramebufferSpecification& GetSpecification() const override;

		VkFramebuffer GetHandle() const { return m_Framebuffer; }

	private:
		void CreateAttachments();
		void CreateFramebuffer();
		void Cleanup();
		
	private:
		friend class VulkanSwapChain;

		VkDevice m_Device;
		VkRenderPass m_RenderPass;

		FramebufferSpecification m_Specification;
		
		VkFramebuffer m_Framebuffer = VK_NULL_HANDLE;
		
		std::vector<VkImage> m_ColorImages;
		std::vector<VkDeviceMemory> m_ColorImageMemories;
		std::vector<VkImageView> m_ColorImageViews;

		VkImage m_DepthImage = VK_NULL_HANDLE;
		VkDeviceMemory m_DepthImageMemory = VK_NULL_HANDLE;
		VkImageView m_DepthImageView = VK_NULL_HANDLE;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};

}
