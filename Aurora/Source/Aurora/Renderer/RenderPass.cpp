#include "aupch.h"
#include "RenderPass.h"

#include "Renderer.h"
#include "Aurora/Core/Assert.h"
#include "Platform/Vulkan/Renderer/VulkanRenderPass.h"

namespace Aurora {

	std::shared_ptr<RenderPass> RenderPass::Create(const RenderPassSpecification& spec) {
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			AU_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::Vulkan:
			return std::make_shared<VulkanRenderPass>(spec);
		}

		AU_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}
