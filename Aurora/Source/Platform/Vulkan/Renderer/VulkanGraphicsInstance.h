#pragma once
#include "Aurora/Renderer/IGraphicsInstance.h"

#include <vulkan/vulkan.h>

namespace Aurora {
	
	class VulkanGraphicsInstance : public IGraphicsInstance {
	public:
		VulkanGraphicsInstance() = default;
		~VulkanGraphicsInstance() override = default;

		void Init() override;
		void Destroy() override;
		void GetDevice() override;

	private:
		VkInstance m_Instance = VK_NULL_HANDLE;
		VkDevice m_Device = VK_NULL_HANDLE;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
	};

}
