#pragma once

#include <optional>

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
		VulkanSwapChain(GLFWwindow* windowHandle);

		void Create(VkDevice* device, VkPhysicalDevice* physicalDevice);
		void Destroy() const;

		void SwapBuffers();

		void Resize();

	public:

		VkSurfaceKHR& GetSurface();
		/// TEMPORARY FOR READING SHADERS
		static std::vector<char> readShaderFile(const std::string& fileName);

	private:
		void createImageViews();
		void createSwapChain();
		void createFramebuffers();
		void createRenderPass();
		void createGraphicsPipeline();

		void cleanupSwapChain() const;

		SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice& device) const;
		QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& device) const;
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;

	private:
		static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

	private:
		GLFWwindow* m_WindowHandle;

		VkSwapchainKHR m_SwapChain;
		std::vector<VkImage> m_SwapChainImages;
		VkFormat m_SwapChainImageFormat;
		VkExtent2D m_SwapChainExtent;
		std::vector<VkImageView> m_SwapChainImageViews;
		std::vector<VkFramebuffer> m_SwapChainFramebuffers;

		VkRenderPass m_RenderPass;
		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_GraphicsPipeline;

		VkSurfaceKHR m_Surface;

		// Pointers to devices in VulkanContext
		VkDevice* m_Device;
		VkPhysicalDevice* m_PhysicalDevice;
	};
}