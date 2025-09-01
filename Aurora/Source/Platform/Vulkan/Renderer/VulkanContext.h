#pragma once
#include "VulkanPipeline.h"

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
	RenderPassLibrary
		RenderPass
		FrameBuffer
	PipelineState: A grafikus pipeline állapotának beállításai (shader programok, renderelési beállítások).
		reference to renderPass
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

		void Init() override;
		void Shutdown() override;
		void SwapBuffers() override;

		uint32_t AcquireNextImage();
		void WaitForCurrentFrameFence();
		void SubmitCommandBuffer();
		
		VkDevice& GetDevice();
		VkPhysicalDevice& GetPhysicalDevice();
		VkQueue& GetGraphicsQueue();
		VkCommandPool& GetCommandPool();
		VkDescriptorPool& GetDescriptorPool();
		VulkanSwapChain& GetSwapChain();
		VkFence& GetFence();

		VkSemaphore& GetSignalSemaphore();

		VkQueue& GetPresentQueue();
		VkCommandBuffer& GetCurrentCommandBuffer();
		uint32_t GetCurrentFrame() const;

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

		void createCommandBuffers();
		void createSyncObjects();
		void recreateSwapChain();

	private:
		friend class VulkanRenderCommand;
		
	private:
		GLFWwindow* m_WindowHandle;
		
		// Vulkan related things
		bool m_AreValidationLayersEnabled;
#ifdef AU_DEBUG
		VulkanValidationLayer m_ValidationLayer;
#endif

		const std::vector<const char*> deviceExtensions = {
			// for rasterized rendering
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,

			// for both rendering
			VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
			VK_KHR_SPIRV_1_4_EXTENSION_NAME,
			VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME,
			VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
			VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME,
			VK_EXT_GRAPHICS_PIPELINE_LIBRARY_EXTENSION_NAME,
			VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME

			// for ray tracing
			// VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
			// VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
			// VK_KHR_RAY_QUERY_EXTENSION_NAME,
			// VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
			// VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
			// VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME,
		};

		VulkanSwapChain m_SwapChain;
		VkInstance m_Instance = VK_NULL_HANDLE;
		VkDevice m_Device = VK_NULL_HANDLE;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
		VkQueue m_GraphicsQueue;
		
		const int MAX_FRAMES_IN_FLIGHT = 3;
		uint32_t currentFrame = 0;
		
		VkQueue presentQueue;

		VkCommandPool m_CommandPool;
		std::vector<VkCommandBuffer> m_CommandBuffers;
		
		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;

		VkDescriptorPool m_DescriptorPool;
	};
}
