#pragma once
#include "VulkanContext.h"
#include "Aurora/Renderer/RenderCommand.h"
#include <vulkan/vulkan.h>

namespace Aurora {
	class VulkanRenderCommand {
	public:
		static void BeginScene();
		static void EndScene();

		// Helper methods for accessing Vulkan resources
		static VkDevice GetDevice();
		static VkPhysicalDevice GetPhysicalDevice();

		static VulkanContext* GetContext() {
			static auto* cached = RenderCommand::GetContextAs<VulkanContext>();
			return cached;
		}
	};
}
