#include "aupch.h"
#include "VulkanSwapChain.h"

#include "VulkanContext.h"
#include "Aurora/Core/Assert.h"
#include "Aurora/Core/Log.h"
#include "GLFW/glfw3.h"

namespace Aurora {

	void VulkanSwapChain::Init(VkDevice* device, VkPhysicalDevice* physicalDevice, VkSurfaceKHR* surface) {
		m_Device = device;
		m_PhysicalDevice = physicalDevice;
		m_Surface = surface;
	}
	
	void VulkanSwapChain::Create(uint32_t width, uint32_t height) {
		createSwapChain(width, height);
		createImageViews();
		createRenderPass();
		createFramebuffers();
	}

	void VulkanSwapChain::Destroy() {
		cleanupSwapChain();

		m_Framebuffers.clear();
		m_RenderPass.reset();
	}

	void VulkanSwapChain::SwapBuffers() {
	}

	void VulkanSwapChain::Resize(uint32_t width, uint32_t height) {
		cleanupSwapChain();

		createSwapChain(width, height);
		createImageViews();
		createFramebuffers();
	}

	std::shared_ptr<Framebuffer> VulkanSwapChain::GetFramebuffer(uint32_t imageIndex) const {
		AU_CORE_ASSERT(imageIndex < m_Framebuffers.size(), "Image index out of range!");
		return m_Framebuffers[imageIndex];
	}

	std::vector<char> VulkanSwapChain::readShaderFile(const std::string& fileName) {
		std::ifstream file(fileName, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}

	SwapChainSupportDetails VulkanSwapChain::querySwapChainSupport(const VkPhysicalDevice& device) const {
		SwapChainSupportDetails details;
		
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, *m_Surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, *m_Surface, &formatCount, nullptr);

		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, *m_Surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, *m_Surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, *m_Surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}



	void VulkanSwapChain::createImageViews() {
		m_SwapChainImageViews.resize(m_SwapChainImages.size());

		for (size_t i = 0; i < m_SwapChainImages.size(); i++) {
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = m_SwapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = m_SwapChainImageFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(*m_Device, &createInfo, nullptr, &m_SwapChainImageViews[i]) != VK_SUCCESS) {
				AU_CORE_CRITICAL("FAILED TO CREATE IMAGE VIEWS!");
			}
		}
	}

	void VulkanSwapChain::createSwapChain(uint32_t width, uint32_t height) {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(*m_PhysicalDevice);

		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent;
		extent.width = width;
		extent.height = height;

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = *m_Surface;

		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = findQueueFamilies(*m_PhysicalDevice);
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		} else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(*m_Device, &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS) {
			AU_CORE_CRITICAL("FAILED TO CREATE SWAP CHAIN!");
		}

		vkGetSwapchainImagesKHR(*m_Device, m_SwapChain, &imageCount, nullptr);
		m_SwapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(*m_Device, m_SwapChain, &imageCount, m_SwapChainImages.data());

		m_SwapChainImageFormat = surfaceFormat.format;
		m_SwapChainExtent = extent;
	}

	void VulkanSwapChain::createFramebuffers() {
		m_Framebuffers.clear();
		m_Framebuffers.reserve(m_SwapChainImages.size());

		for (size_t i = 0; i < m_SwapChainImages.size(); i++) {
			// Create framebuffer specification for SwapChain
			FramebufferSpecification fbSpec;
			fbSpec.Width = m_SwapChainExtent.width;
			fbSpec.Height = m_SwapChainExtent.height;
			fbSpec.SwapChainTarget = true;

			// Add color attachment
			FramebufferTextureSpecification colorAttachment;
			colorAttachment.TextureFormat = FramebufferTextureFormat::RGBA8; // TODO: Convert from VkFormat
			fbSpec.Attachments = { colorAttachment };

			// Create framebuffer
			auto framebuffer = std::make_shared<VulkanFramebuffer>(fbSpec);

			// Set external image view (SwapChain image)
			std::vector<VkImageView> imageViews = { m_SwapChainImageViews[i] };
			framebuffer->SetExternalImageViews(imageViews);
			framebuffer->SetCompatibleRenderPass(m_RenderPass);

			m_Framebuffers.push_back(framebuffer);
		}
	}

	void VulkanSwapChain::createRenderPass() {
		// Create RenderPass specification
		RenderPassSpecification renderPassSpec("SwapChainRenderPass");
		renderPassSpec.attachments.clear();

		// Add color attachment
		RenderPassAttachmentDesc colorAttachment;
		colorAttachment.name = "SwapChainColor";
		colorAttachment.format = FramebufferTextureFormat::RGBA8; // TODO: Convert from VkFormat
		colorAttachment.loadOp = AttachmentLoadOp::Clear;
		colorAttachment.storeOp = AttachmentStoreOp::Store;
		colorAttachment.initialLayout = ImageLayout::Undefined;
		colorAttachment.finalLayout = ImageLayout::Present;
		renderPassSpec.attachments.push_back(colorAttachment);

		// Create subpass
		SubpassInfo subpass;
		subpass.colorAttachments.push_back(0);
		renderPassSpec.subpasses.clear();
		renderPassSpec.subpasses.push_back(subpass);

		m_RenderPass = std::make_shared<VulkanRenderPass>(renderPassSpec);
	}

	void VulkanSwapChain::cleanupSwapChain() {
		vkDeviceWaitIdle(*m_Device);

		// Clear framebuffers
		m_Framebuffers.clear();

		// Destroy image views
		for (auto& imageView : m_SwapChainImageViews) {
			vkDestroyImageView(*m_Device, imageView, nullptr);
		}
		m_SwapChainImageViews.clear();

		// Destroy SwapChain
		if (m_SwapChain != VK_NULL_HANDLE) {
			vkDestroySwapchainKHR(*m_Device, m_SwapChain, nullptr);
			m_SwapChain = VK_NULL_HANDLE;
		}
	}

	QueueFamilyIndices VulkanSwapChain::findQueueFamilies(const VkPhysicalDevice& device) const {
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, *m_Surface, &presentSupport);

			if (presentSupport) {
				indices.presentFamily = i;
			}

			if (indices.isComplete()) {
				break;
			}

			i++;
		}

		return indices;
	}

	VkSurfaceFormatKHR VulkanSwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}
		return availableFormats[0];
	}

	VkPresentModeKHR VulkanSwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}
	
}
