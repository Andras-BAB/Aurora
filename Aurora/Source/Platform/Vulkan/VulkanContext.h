#pragma once

#ifdef DEBUG
#include "Platform/Vulkan/VulkanValidationLayer.h"
#else
#include "vulkan/vulkan.h"
#endif

#include <optional>
#include <glm/glm.hpp>

#include "Platform/Windows/WindowsWindow.h"
#include "Platform/Vulkan/VulkanSwapChain.h"
#include "Renderer/GraphicsContext.h"

namespace Aurora {
	struct Vertex {
		glm::vec2 pos;
		glm::vec3 color;

		static VkVertexInputBindingDescription getBindingDescription() {
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);

			return attributeDescriptions;
		}
	};

	class VulkanContext : public GraphicsContext {
	public:
		VulkanContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void Shutdown() override;
		virtual void SwapBuffers() override;

	private:

		void createVulkanInstance();
		std::vector<const char*> getRequiredExtensions() const;
		void createSurface();

		void pickPhysicalDevice();
		bool isDeviceSuitable(const VkPhysicalDevice& device);
		QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& device);
		bool checkDeviceExtensionSupport(const VkPhysicalDevice& device) const;
		SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice& device);

		void createLogicalDevice();

		void createImageViews();
		void createRenderPass();
		void createGraphicsPipeline();
		VkShaderModule createShaderModule(const std::vector<char>& code);
		static std::vector<char> readFile(const std::string& filename);

		void createFramebuffers();
		void createCommandPool();
		void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
		void createVertexBuffer();
		void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
						  VkMemoryPropertyFlags properties, VkBuffer& buffer,
						  VkDeviceMemory& bufferMemory);
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

		void createIndexBuffer();

		void createCommandBuffers();
		void createSyncObjects();
		void recreateSwapChain();
		void cleanupSwapChain();

	private:
		GLFWwindow* m_WindowHandle;

		const std::vector<Vertex> vertices = {
			{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
			{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
		};

		const std::vector<uint16_t> indices = {
			0, 1, 2, 2, 3, 0
		};

		// Vulkan related things
		bool m_AreValidationLayersEnabled;
#ifdef DEBUG
		VulkanValidationLayer m_ValidationLayer;
#endif

		VulkanSwapChain m_SwapChain;

		constexpr std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		VkInstance m_Instance;
		VkDevice m_Device;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;




		const int MAX_FRAMES_IN_FLIGHT = 2;

		uint32_t currentFrame = 0;

		//VkPhysicalDevice physicalDevice = VK_NULL_HANDLE; //

		VkQueue graphicsQueue;

		//VkSurfaceKHR surface; //
		VkQueue presentQueue;

		//VkSwapchainKHR swapChain; //
		//std::vector<VkImage> swapChainImages; //
		//VkFormat swapChainImageFormat; //
		//VkExtent2D swapChainExtent; //
		//std::vector<VkImageView> swapChainImageViews; //

		VkRenderPass renderPass;
		VkPipelineLayout pipelineLayout;
		VkPipeline graphicsPipeline;

		//std::vector<VkFramebuffer> swapChainFramebuffers; //
		VkCommandPool commandPool;

		std::vector<VkCommandBuffer> commandBuffers;

		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;

		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
	};
}
