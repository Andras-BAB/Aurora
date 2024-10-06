#include "aupch.h"
#include "Aurora/Renderer/VertexArray.h"

#include "Aurora/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Aurora {

	std::shared_ptr<VertexArray> VertexArray::Create() {
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			AU_CORE_ERROR("RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLVertexArray>();
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