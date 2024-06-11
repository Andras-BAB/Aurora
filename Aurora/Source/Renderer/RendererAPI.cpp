#include "aupch.h"

#include "Platform/Vulkan/VulkanRendererAPI.h"
#include "Renderer/RendererAPI.h"

namespace Aurora {

	RendererAPI::API RendererAPI::s_API = RendererAPI::API::Vulkan;

	std::unique_ptr<RendererAPI> RendererAPI::Create() {
		switch (s_API) {
		case RendererAPI::API::None:
			std::cerr << "Unknown Render API!";
			return nullptr;

		case RendererAPI::API::OpenGL:
			std::cerr << "Currently not supported OpenGL!";
			return nullptr;

		case RendererAPI::API::Vulkan:
			return std::make_unique<VulkanRendererAPI>();
		}

		std::cerr << "Unknown Render API!";
		return nullptr;
	}

}