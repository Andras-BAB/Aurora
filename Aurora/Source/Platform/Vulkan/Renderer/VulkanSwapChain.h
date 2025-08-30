#pragma once

#include "VulkanPipeline.h"

#include <optional>
#include <glm/glm.hpp>

#include "Platform/Windows/WindowsWindow.h"
#include "vulkan/vulkan.h"

namespace Aurora {
	
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() const {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class VulkanSwapChain {
	public:
		VulkanSwapChain() = default;

		void Init(VkDevice* device, VkPhysicalDevice* physicalDevice, VkSurfaceKHR* surface);
		void Create(uint32_t width, uint32_t height);
		void Destroy();

		void SwapBuffers();

		void Resize(uint32_t width, uint32_t height);

	public:
		VkSwapchainKHR& GetSwapChain();
		VkRenderPass& GetRenderPass();
		VkExtent2D& GetExtent();
		std::vector<VkFramebuffer>& GetFrameBuffers();
		VulkanPipeline& GetPipeline();
		
		/// TEMPORARY FOR READING SHADERS
		static std::vector<char> readShaderFile(const std::string& fileName);
		void cleanupSwapChain() const;

		SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice& device) const;
	private:
		void createImageViews();
		void createSwapChain(uint32_t width, uint32_t height);
		void createFramebuffers();
		void createRenderPass();

		QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& device) const;
		
	private:
		static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

	private:
		friend class VulkanContext;
		GLFWwindow* m_WindowHandle;

		VkSwapchainKHR m_SwapChain;
		VkFormat m_SwapChainImageFormat;
		std::vector<VkImage> m_SwapChainImages;
		VkExtent2D m_SwapChainExtent;
		std::vector<VkImageView> m_SwapChainImageViews;

		std::vector<VkFramebuffer> m_SwapChainFramebuffers;
		VkRenderPass m_RenderPass;
		VulkanPipeline m_Pipeline;
		
		// Pointers to devices in VulkanContext
		VkDevice* m_Device;
		VkPhysicalDevice* m_PhysicalDevice;
		VkSurfaceKHR* m_Surface;
	};
}