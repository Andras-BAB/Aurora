#include "aupch.h"

#include "Renderer/RendererAPI.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Aurora {

	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;

	std::unique_ptr<RendererAPI> RendererAPI::Create() {
		switch (s_API) {
		case RendererAPI::API::None:
			AU_CORE_LOG_ERROR("Unknown Render API!");
			return nullptr;

		case RendererAPI::API::OpenGL:
			return std::make_unique<OpenGLRendererAPI>();

		case RendererAPI::API::Vulkan:
			AU_CORE_LOG_ERROR("RendererAPI::Vulkan is currently not supported!");

		case RendererAPI::API::DirectX11:
			AU_CORE_LOG_ERROR("RendererAPI::DirectX11 is currently not supported!");
			return nullptr;
		}

		AU_CORE_LOG_ERROR("Unknown Render API!");
		return nullptr;
	}

}
