#include "aupch.h"
#include "Aurora/Renderer/Buffer.h"

#include "Aurora/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"
#include "Platform/Vulkan/Renderer/VulkanBuffer.h"

namespace Aurora {

	std::shared_ptr<VertexBuffer> VertexBuffer::Create(uint32_t size) {
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			AU_CORE_ERROR("RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLVertexBuffer>(size);
		case RendererAPI::API::Vulkan:
			return std::make_shared<VulkanVertexBuffer>(size);
		case RendererAPI::API::DirectX12:
			AU_CORE_ERROR("RendererAPI::DirectX12 is currently not supported!");
			return nullptr;
		case RendererAPI::API::DirectX11:
			AU_CORE_ERROR("RendererAPI::DirectX11 is currently not supported!");
			return nullptr;
		}

		AU_CORE_ERROR("Unknown RendererAPI!");
		return nullptr;
	}

	std::shared_ptr<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size) {
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			AU_CORE_ERROR("RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLVertexBuffer>(vertices, size);
		case RendererAPI::API::Vulkan:
			return std::make_shared<VulkanVertexBuffer>(vertices, size);
		case RendererAPI::API::DirectX12:
			AU_CORE_ERROR("RendererAPI::DirectX12 is currently not supported!");
			return nullptr;
		case RendererAPI::API::DirectX11:
			AU_CORE_ERROR("RendererAPI::DirectX11 is currently not supported!");
			return nullptr;
		}

		AU_CORE_ERROR("Unknown RendererAPI!");
		return nullptr;
	}

	std::shared_ptr<IndexBuffer> IndexBuffer::Create(uint32_t size) {
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			AU_CORE_ERROR("RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLIndexBuffer>(size);
		case RendererAPI::API::Vulkan:
			return std::make_shared<VulkanIndexBuffer>(size);
		case RendererAPI::API::DirectX12:
			AU_CORE_ERROR("RendererAPI::DirectX12 is currently not supported!");
			return nullptr;
		case RendererAPI::API::DirectX11:
			AU_CORE_ERROR("RendererAPI::DirectX11 is currently not supported!");
			return nullptr;
		}
		AU_CORE_ERROR("Unknown RendererAPI!");
		return nullptr;
	}

	std::shared_ptr<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t size) {
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			AU_CORE_ERROR("RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return std::make_shared<OpenGLIndexBuffer>(indices, size);
		case RendererAPI::API::Vulkan:
			return std::make_shared<VulkanIndexBuffer>(indices, size);
		case RendererAPI::API::DirectX12:
			AU_CORE_ERROR("RendererAPI::DirectX12 is currently not supported!");
			return nullptr;
		case RendererAPI::API::DirectX11:
			AU_CORE_ERROR("RendererAPI::DirectX11 is currently not supported!");
			return nullptr;
		}

		AU_CORE_ERROR("Unknown RendererAPI!");
		return nullptr;
	}

}
