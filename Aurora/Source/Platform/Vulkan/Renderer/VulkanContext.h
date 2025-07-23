#pragma once

#ifdef AU_DEBUG
#include "VulkanValidationLayer.h"
#else
#include "vulkan/vulkan.h"
#endif

#include <optional>
#include <glm/glm.hpp>

#include "VulkanSwapChain.h"
#include "Platform/Windows/WindowsWindow.h"
#include "Aurora/Renderer/GraphicsContext.h"
#include "Aurora/Scene/MeshAsset.h"

/*
 *
GraphicsContext
	SwapChain: Az ablakok közötti képkockák cseréjének kezelése.
	Device: A grafikus eszközök (GPU) és azok erőforrásainak inicializálása és kezelése.
	CommandQueue: A parancsok sorba állítása és végrehajtása.
	Surface: Az ablak felületének és a megjelenítés kezelésének inicializálása.
	Fence: Szinkronizációs primitívek, amelyek biztosítják, hogy a CPU és a GPU megfelelő sorrendben hajtsák végre a műveleteket.
	CommandPool
	CommandBuffer
RendererAPI
	PipelineState: A grafikus pipeline állapotának beállításai (shader programok, renderelési beállítások).
	RenderCommands: A renderelési parancsok meghatározása és végrehajtása.
	Shader API: A shader programok kezelése és használata.
	Texture API: A textúrák betöltése és kezelése.
	Buffer API: A vertex, index és egyéb buffer erőforrások kezelése.
 * 
 */

namespace Aurora {

	class VulkanContext : public GraphicsContext {
	public:
		VulkanContext(GLFWwindow* windowHandle);

		virtual void Init() override;
		virtual void Shutdown() override;
		virtual void SwapBuffers() override;

		VkDevice& GetDevice();
		VkPhysicalDevice& GetPhysicalDevice();
		VkQueue& GetGraphicsQueue();
		VkCommandPool& GetCommandPool();
		
	private:
		friend class VulkanRendererAPI;
		friend class ImGuiLayer;
		
		void createVulkanInstance();
		std::vector<const char*> getRequiredExtensions();
		void createSurface();

		void pickPhysicalDevice();
		bool isDeviceSuitable(VkPhysicalDevice device);
		QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);

		void createLogicalDevice();
		
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

	private:
		GLFWwindow* m_WindowHandle;

		const std::vector<Vertex> vertices = {
			{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
			{{-0.5f, 0.5f}, {1.0f, 0.0f, 1.0f}}
		};

		const std::vector<uint16_t> indices = {
			0, 1, 2, 2, 3, 0
		};

		// Vulkan related things
		bool m_AreValidationLayersEnabled;
#ifdef AU_DEBUG
		VulkanValidationLayer m_ValidationLayer;
#endif

		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			//VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
			//VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
			//VK_KHR_RAY_QUERY_EXTENSION_NAME,
			//VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
			//VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
			//VK_KHR_SPIRV_1_4_EXTENSION_NAME,
			//VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME,
			//VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
			//VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
			//VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME,
			//VK_EXT_GRAPHICS_PIPELINE_LIBRARY_EXTENSION_NAME
		};

		VulkanSwapChain m_SwapChain;
		VkInstance m_Instance = VK_NULL_HANDLE;
		VkDevice m_Device = VK_NULL_HANDLE;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
		VkQueue m_GraphicsQueue;
		
		const int MAX_FRAMES_IN_FLIGHT = 2;
		uint32_t currentFrame = 0;
		
		VkQueue presentQueue;

		VkCommandPool m_CommandPool;
		std::vector<VkCommandBuffer> m_CommandBuffers;
		
		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;

		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
		std::shared_ptr<MeshAsset> m_Mesh;
	};
}
