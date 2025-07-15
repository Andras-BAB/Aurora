#include "aupch.h"

#include "Aurora/Renderer/RendererAPI.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"
#include "Platform/Vulkan/Renderer/VulkanRendererAPI.h"

namespace Aurora {

	RendererAPI::API RendererAPI::s_API = RendererAPI::API::Vulkan;

	std::unique_ptr<RendererAPI> RendererAPI::Create() {
		switch (s_API) {
		case RendererAPI::API::None:
			AU_CORE_ERROR("Unknown Render API!");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return std::make_unique<OpenGLRendererAPI>();

			case RendererAPI::API::Vulkan:
			return std::make_unique<VulkanRendererAPI>();

		case RendererAPI::API::DirectX11:
			AU_CORE_ERROR("RendererAPI::DirectX11 is currently not supported!");
			return nullptr;
		case RendererAPI::API::DirectX12:
			AU_CORE_ERROR("RendererAPI::DirectX11 is currently not supported!");
			return nullptr;
		}

		AU_CORE_ERROR("Unknown Render API!");
		return nullptr;
	}

}
