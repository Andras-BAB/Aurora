#pragma once
#include "Aurora/Renderer/Framebuffer.h"

#include <vector>
#include <memory>
#include <vulkan/vulkan.h>

namespace Aurora {

	class VulkanRenderPass;

	class VulkanFramebuffer : public Framebuffer {
	public:
		VulkanFramebuffer(const FramebufferSpecification& spec);
		virtual ~VulkanFramebuffer();

		void Bind() override;
		void Unbind() override;

		void Invalidate() override;

		void Resize(uint32_t width, uint32_t height) override;
		[[deprecated]] int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

		void ClearAttachment(uint32_t attachmentIndex, int value) override;

		uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override;

		const FramebufferSpecification& GetSpecification() const override;

		VkFramebuffer GetVulkanFramebuffer() const { return m_Framebuffer; }
		std::vector<VkImageView> GetImageViews() const;

		// SwapChain támogatáshoz
		void SetExternalImageViews(const std::vector<VkImageView>& imageViews);
		void SetCompatibleRenderPass(std::shared_ptr<VulkanRenderPass> renderPass);

	private:
		void CreateAttachments();
		void CreateFramebuffer();
		void Cleanup();

		VkFormat ToVulkanFormat(FramebufferTextureFormat format);
		bool IsDepthFormat(FramebufferTextureFormat format);

	private:
		VkDevice m_Device = VK_NULL_HANDLE;
		std::shared_ptr<VulkanRenderPass> m_RenderPass;

		FramebufferSpecification m_Specification;

		VkFramebuffer m_Framebuffer = VK_NULL_HANDLE;

		// Saját képek (nem SwapChain esetén)
		std::vector<VkImage> m_ColorImages;
		std::vector<VkDeviceMemory> m_ColorImageMemories;
		std::vector<VkImageView> m_ColorImageViews;

		VkImage m_DepthImage = VK_NULL_HANDLE;
		VkDeviceMemory m_DepthImageMemory = VK_NULL_HANDLE;
		VkImageView m_DepthImageView = VK_NULL_HANDLE;

		// Külső képek (SwapChain esetén)
		std::vector<VkImageView> m_ExternalImageViews;
		bool m_UsingExternalImages = false;

		std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
		FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
	};

}
