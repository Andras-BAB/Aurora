#include "aupch.h"

#include "IFramebuffer.h"

#include "Renderer.h"
#include "Aurora/Core/Log.h"

#include "Platform/DirectX/Renderer/DirectX12Framebuffer.h"

namespace Aurora {
	
	std::shared_ptr<IFramebuffer> IFramebuffer::Create(const FramebufferSpecification& spec) {
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			AU_CORE_ERROR("RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::DirectX12:
			return std::make_shared<DirectX12Framebuffer>(spec);
		case RendererAPI::API::Vulkan:
			AU_CORE_ERROR("RendererAPI::Vulkan is currently not supported!");
			return nullptr;
		case RendererAPI::API::DirectX11:
			AU_CORE_ERROR("RendererAPI::DirectX11 is currently not supported!");
			return nullptr;
		}

		AU_CORE_ERROR("Unknown RendererAPI!");
		return nullptr;
	}
	
}

