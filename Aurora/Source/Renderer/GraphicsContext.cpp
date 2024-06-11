#include "aupch.h"

#include "Renderer/GraphicsContext.h"

#include "Renderer.h"
#include "RendererAPI.h"
#include "Platform/Vulkan/VulkanContext.h"
#include "Platform/Windows/WindowsWindow.h"

namespace Aurora {

	std::unique_ptr<GraphicsContext> GraphicsContext::Create(void* window) {
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			std::cerr << "RendererAPI::None is currently not supported!";
			return nullptr;

		case RendererAPI::API::OpenGL:
			std::cerr << "RendererAPI::OpenGL is currently not supported!";
			return nullptr;

		case RendererAPI::API::Vulkan:
			return std::make_unique<VulkanContext>(static_cast<GLFWwindow*>(window));
		}

		std::cerr << "Unknown RendererAPI!";
		return nullptr;
	}

}
