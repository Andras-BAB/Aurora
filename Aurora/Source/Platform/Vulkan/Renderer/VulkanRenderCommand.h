#pragma once
#include "VulkanContext.h"
#include "Aurora/Renderer/RenderCommand.h"

namespace Aurora {
	class VulkanRenderCommand {
	public:
		static void BeginScene();
		
		static void EndScene();

		static VulkanContext* GetContext() {
			static auto* cached = RenderCommand::GetContextAs<VulkanContext>();
			return cached;
		}

		static VkDevice GetDevice() {
			return GetContext()->GetDevice();
		}
	};
}
