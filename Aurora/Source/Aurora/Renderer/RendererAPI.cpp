#include "aupch.h"

#include "Aurora/Renderer/RendererAPI.h"

#include "Platform/DirectX/Renderer/DirectX12RendererAPI.h"

namespace Aurora {

	RendererAPI::API RendererAPI::s_API = RendererAPI::API::DirectX12;

	std::unique_ptr<RendererAPI> RendererAPI::Create() {
		switch (s_API) {
		case RendererAPI::API::None:
			AU_CORE_ERROR("Unknown Render API!");
			return nullptr;
		case RendererAPI::API::Vulkan:
			AU_CORE_ERROR("RendererAPI::Vulkan is currently not supported!");
			return nullptr;
		case RendererAPI::API::DirectX11:
			AU_CORE_ERROR("RendererAPI::DirectX11 is currently not supported!");
			return nullptr;
		case RendererAPI::API::DirectX12:
			return std::make_unique<DirectX12RendererAPI>();
		}

		AU_CORE_ERROR("Unknown Render API!");
		return nullptr;
	}

}
