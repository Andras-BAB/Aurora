#include "aupch.h"

#include "IGraphicsInstance.h"
#include "Renderer.h"
#include "Platform/Vulkan/Renderer/VulkanGraphicsInstance.h"

namespace Aurora {
	std::unique_ptr<IGraphicsInstance> IGraphicsInstance::Create() {
		switch (Renderer::GetAPI()) {
			case RendererAPI::API::None:
				AU_CORE_ERROR("RendererAPI::None is currently not supported!");
				return nullptr;
			case RendererAPI::API::OpenGL:
				AU_CORE_ERROR("RendererAPI::OpenGL is discontinued! Use Vulkan instead!");
				return nullptr;
			case RendererAPI::API::Vulkan:
				return std::make_unique<VulkanGraphicsInstance>();
			case RendererAPI::API::DirectX11:
				AU_CORE_ERROR("RendererAPI::DirectX11 is currently not supported!");
			default:
				AU_CORE_ERROR("Unknown RendererAPI!");
				return nullptr;
		}
	}

}
