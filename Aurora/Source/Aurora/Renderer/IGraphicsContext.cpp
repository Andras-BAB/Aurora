#include "aupch.h"

#include "Aurora/Renderer/IGraphicsContext.h"

#include "Renderer.h"
#include "RendererAPI.h"
#include "Aurora/Core/Log.h"
#include "Platform/DirectX/Renderer/DirectX12Context.h"

#include "Platform/Windows/WindowsWindow.h"

namespace Aurora {

	std::unique_ptr<IGraphicsContext> IGraphicsContext::Create(void* window) {
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			AU_CORE_ERROR("RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::Vulkan:
			AU_CORE_ERROR("RendererAPI::Vulkan is currently not supported!");
			return nullptr;
		case RendererAPI::API::DirectX11:
			AU_CORE_ERROR("RendererAPI::DirectX11 is currently not supported!");
		case RendererAPI::API::DirectX12:
			return std::make_unique<DirectX12Context>(static_cast<WindowsWindow*>(window));
		}

		std::cerr << "Unknown RendererAPI!";
		return nullptr;
	}

}
