#pragma once

#include "Aurora/Renderer/RenderPass.h"
#include "Aurora/Renderer/Framebuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanFramebuffer.h"

#include <optional>
#include <memory>
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

		// Új API független interfész
		std::shared_ptr<RenderPass> GetRenderPass() const { return m_RenderPass; }
		std::shared_ptr<Framebuffer> GetFramebuffer(uint32_t imageIndex) const;
		uint32_t GetImageCount() const { return static_cast<uint32_t>(m_SwapChainImages.size()); }

		// Vulkan specifikus getterek (backward compatibility)
		VkSwapchainKHR& GetHandle() { return m_SwapChain; }
		VkExtent2D& GetExtent() { return m_SwapChainExtent; }

		/// TEMPORARY FOR READING SHADERS
		static std::vector<char> readShaderFile(const std::string& fileName);
		SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice& device) const;

	private:
		void createImageViews();
		void createSwapChain(uint32_t width, uint32_t height);
		void createFramebuffers();
		void createRenderPass();
		void cleanupSwapChain();

		QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& device) const;

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

		// Új architektúra
		std::shared_ptr<VulkanRenderPass> m_RenderPass;
		std::vector<std::shared_ptr<VulkanFramebuffer>> m_Framebuffers;
		
		// Pointers to devices in VulkanContext
		VkDevice* m_Device;
		VkPhysicalDevice* m_PhysicalDevice;
		VkSurfaceKHR* m_Surface;
	};
}