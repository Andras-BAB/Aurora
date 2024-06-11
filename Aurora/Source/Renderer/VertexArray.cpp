#include "aupch.h"
#include "Renderer/VertexArray.h"

#include "Platform/Vulkan/VulkanVertexArray.h"
#include "Renderer/Renderer.h"
//#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Aurora {

	std::shared_ptr<VertexArray> VertexArray::Create() {
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			AU_CORE_LOG_ERROR("RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			//return std::make_shared<OpenGLVertexArray>();
			AU_CORE_LOG_ERROR("RendererAPI::OpenGL is currently not supported!");
			return nullptr;
		case RendererAPI::API::Vulkan:
			return std::make_shared<VulkanVertexArray>();
		}

		AU_CORE_LOG_ERROR("Unknown RendererAPI!");
		return nullptr;
	}

}