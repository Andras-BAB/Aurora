#include "aupch.h"
#include "VulkanFramebuffer.h"

#include "VulkanRenderCommand.h"
#include "VulkanRenderPass.h"
#include "Aurora/Core/Assert.h"
#include "Aurora/Core/Log.h"
#include "Platform/Vulkan/Utils/VulkanHelper.h"

namespace Aurora {
	VulkanFramebuffer::VulkanFramebuffer(const FramebufferSpecification& spec) 
		: m_Specification(spec) {
		m_Device = VulkanRenderCommand::GetDevice();

		// Parse attachment specifications
		for (const auto& attachment : m_Specification.Attachments.Attachments) {
			if (IsDepthFormat(attachment.TextureFormat)) {
				m_DepthAttachmentSpecification = attachment;
			} else {
				m_ColorAttachmentSpecifications.push_back(attachment);
			}
		}

		Invalidate();
	}

	VulkanFramebuffer::~VulkanFramebuffer() {
		Cleanup();
	}

	void VulkanFramebuffer::Bind() {
		// Vulkan-ban a framebuffer binding a render pass kezdésekor történik
		// Itt nem kell semmit csinálni
	}

	void VulkanFramebuffer::Unbind() {
		// Vulkan-ban a framebuffer unbinding a render pass végén történik
		// Itt nem kell semmit csinálni
	}

	void VulkanFramebuffer::Invalidate() {
		if (m_Framebuffer != VK_NULL_HANDLE) {
			Cleanup();
		}

		if (m_Specification.Width == 0 || m_Specification.Height == 0) {
			AU_CORE_WARN("Framebuffer size is 0!");
			return;
		}

		if (!m_UsingExternalImages) {
			CreateAttachments();
		}

		// Framebuffer csak akkor hozható létre, ha van render pass
		if (m_RenderPass) {
			CreateFramebuffer();
		}
	}

	void VulkanFramebuffer::Resize(uint32_t width, uint32_t height) {
		if (m_Specification.Width == width && m_Specification.Height == height) {
			return;
		}

		m_Specification.Width = width;
		m_Specification.Height = height;
		Invalidate();
	}

	int VulkanFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y) {
		// TODO: Implement pixel reading from Vulkan image
		AU_CORE_WARN("VulkanFramebuffer::ReadPixel not implemented yet!");
		return 0;
	}

	void VulkanFramebuffer::ClearAttachment(uint32_t attachmentIndex, int value) {
		// TODO: Implement attachment clearing
		AU_CORE_WARN("VulkanFramebuffer::ClearAttachment not implemented yet!");
	}

	uint32_t VulkanFramebuffer::GetColorAttachmentRendererID(uint32_t index) const {
		AU_CORE_ASSERT(index < m_ColorAttachments.size(), "Index out of range!");
		return m_ColorAttachments[index];
	}

	const FramebufferSpecification& VulkanFramebuffer::GetSpecification() const {
		return m_Specification;
	}

	std::vector<VkImageView> VulkanFramebuffer::GetImageViews() const {
		if (m_UsingExternalImages) {
			return m_ExternalImageViews;
		}

		std::vector<VkImageView> imageViews;

		// Add color attachments
		for (const auto& colorView : m_ColorImageViews) {
			imageViews.push_back(colorView);
		}

		// Add depth attachment if exists
		if (m_DepthImageView != VK_NULL_HANDLE) {
			imageViews.push_back(m_DepthImageView);
		}

		return imageViews;
	}

	void VulkanFramebuffer::SetExternalImageViews(const std::vector<VkImageView>& imageViews) {
		m_ExternalImageViews = imageViews;
		m_UsingExternalImages = true;

		// Recreate framebuffer with external images
		if (m_RenderPass) {
			CreateFramebuffer();
		}
	}

	void VulkanFramebuffer::SetCompatibleRenderPass(std::shared_ptr<VulkanRenderPass> renderPass) {
		m_RenderPass = renderPass;

		// Recreate framebuffer with the new render pass
		if (m_Framebuffer != VK_NULL_HANDLE) {
			vkDestroyFramebuffer(m_Device, m_Framebuffer, nullptr);
			m_Framebuffer = VK_NULL_HANDLE;
		}

		CreateFramebuffer();
	}

	void VulkanFramebuffer::CreateAttachments() {
		// Cleanup existing attachments
		for (auto& imageView : m_ColorImageViews) {
			if (imageView != VK_NULL_HANDLE) {
				vkDestroyImageView(m_Device, imageView, nullptr);
			}
		}
		for (auto& image : m_ColorImages) {
			if (image != VK_NULL_HANDLE) {
				vkDestroyImage(m_Device, image, nullptr);
			}
		}
		for (auto& memory : m_ColorImageMemories) {
			if (memory != VK_NULL_HANDLE) {
				vkFreeMemory(m_Device, memory, nullptr);
			}
		}

		m_ColorImages.clear();
		m_ColorImageMemories.clear();
		m_ColorImageViews.clear();
		m_ColorAttachments.clear();

		// Create color attachments
		for (size_t i = 0; i < m_ColorAttachmentSpecifications.size(); ++i) {
			const auto& spec = m_ColorAttachmentSpecifications[i];

			VkImage image;
			VkDeviceMemory imageMemory;
			VkImageView imageView;

			// Create image
			VkImageCreateInfo imageInfo{};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = m_Specification.Width;
			imageInfo.extent.height = m_Specification.Height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = ToVulkanFormat(spec.TextureFormat);
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			VK_CHECK(vkCreateImage(m_Device, &imageInfo, nullptr, &image));

			// Allocate memory
			VkMemoryRequirements memRequirements;
			vkGetImageMemoryRequirements(m_Device, image, &memRequirements);

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = vkhelper::findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			VK_CHECK(vkAllocateMemory(m_Device, &allocInfo, nullptr, &imageMemory));
			VK_CHECK(vkBindImageMemory(m_Device, image, imageMemory, 0));

			// Create image view
			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = image;
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = ToVulkanFormat(spec.TextureFormat);
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			VK_CHECK(vkCreateImageView(m_Device, &viewInfo, nullptr, &imageView));

			m_ColorImages.push_back(image);
			m_ColorImageMemories.push_back(imageMemory);
			m_ColorImageViews.push_back(imageView);
			m_ColorAttachments.push_back(static_cast<uint32_t>(i)); // Simple ID for now
		}

		// Create depth attachment if specified
		if (m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None) {
			VkImageCreateInfo imageInfo{};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = m_Specification.Width;
			imageInfo.extent.height = m_Specification.Height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = ToVulkanFormat(m_DepthAttachmentSpecification.TextureFormat);
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

			VK_CHECK(vkCreateImage(m_Device, &imageInfo, nullptr, &m_DepthImage));

			VkMemoryRequirements memRequirements;
			vkGetImageMemoryRequirements(m_Device, m_DepthImage, &memRequirements);

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = vkhelper::findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			VK_CHECK(vkAllocateMemory(m_Device, &allocInfo, nullptr, &m_DepthImageMemory));
			VK_CHECK(vkBindImageMemory(m_Device, m_DepthImage, m_DepthImageMemory, 0));

			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = m_DepthImage;
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = ToVulkanFormat(m_DepthAttachmentSpecification.TextureFormat);
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			VK_CHECK(vkCreateImageView(m_Device, &viewInfo, nullptr, &m_DepthImageView));
		}
	}

	void VulkanFramebuffer::CreateFramebuffer() {
		if (!m_RenderPass) {
			AU_CORE_ERROR("Cannot create framebuffer without render pass!");
			return;
		}

		std::vector<VkImageView> attachments = GetImageViews();

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_RenderPass->GetVulkanRenderPass();
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = m_Specification.Width;
		framebufferInfo.height = m_Specification.Height;
		framebufferInfo.layers = 1;

		VK_CHECK(vkCreateFramebuffer(m_Device, &framebufferInfo, nullptr, &m_Framebuffer));
	}

	void VulkanFramebuffer::Cleanup() {
		if (m_Framebuffer != VK_NULL_HANDLE) {
			vkDestroyFramebuffer(m_Device, m_Framebuffer, nullptr);
			m_Framebuffer = VK_NULL_HANDLE;
		}

		// Cleanup depth resources
		if (m_DepthImageView != VK_NULL_HANDLE) {
			vkDestroyImageView(m_Device, m_DepthImageView, nullptr);
			m_DepthImageView = VK_NULL_HANDLE;
		}
		if (m_DepthImage != VK_NULL_HANDLE) {
			vkDestroyImage(m_Device, m_DepthImage, nullptr);
			m_DepthImage = VK_NULL_HANDLE;
		}
		if (m_DepthImageMemory != VK_NULL_HANDLE) {
			vkFreeMemory(m_Device, m_DepthImageMemory, nullptr);
			m_DepthImageMemory = VK_NULL_HANDLE;
		}

		// Cleanup color resources (only if not using external images)
		if (!m_UsingExternalImages) {
			for (auto& imageView : m_ColorImageViews) {
				if (imageView != VK_NULL_HANDLE) {
					vkDestroyImageView(m_Device, imageView, nullptr);
				}
			}
			for (auto& image : m_ColorImages) {
				if (image != VK_NULL_HANDLE) {
					vkDestroyImage(m_Device, image, nullptr);
				}
			}
			for (auto& memory : m_ColorImageMemories) {
				if (memory != VK_NULL_HANDLE) {
					vkFreeMemory(m_Device, memory, nullptr);
				}
			}
		}

		m_ColorImageViews.clear();
		m_ColorImages.clear();
		m_ColorImageMemories.clear();
	}

	VkFormat VulkanFramebuffer::ToVulkanFormat(FramebufferTextureFormat format) {
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

	bool VulkanFramebuffer::IsDepthFormat(FramebufferTextureFormat format) {
		switch (format) {
		case FramebufferTextureFormat::DEPTH24STENCIL8:
			return true;
		default:
			return false;
		}
	}
	
}
