#include "aupch.h"
#include "Renderer/Buffer.h"

#include "Renderer/Renderer.h"

#include "Platform/Vulkan/VulkanBuffer.h"

namespace Aurora {

	std::shared_ptr<VertexBuffer> VertexBuffer::Create(uint32_t size) {
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			AU_CORE_LOG_ERROR("RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			AU_CORE_LOG_ERROR("RendererAPI::OpenGL is currently not supported!");
			return nullptr;
		case RendererAPI::API::Vulkan:
			return std::make_shared<VulkanVertexBuffer>(size);
		}

		AU_CORE_LOG_ERROR("Unknown RendererAPI!");
		return nullptr;
	}

	std::shared_ptr<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size) {
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			AU_CORE_LOG_ERROR("RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			AU_CORE_LOG_ERROR("RendererAPI::OpenGL is currently not supported!");
			return nullptr;
		case RendererAPI::API::Vulkan:
			return std::make_shared<VulkanVertexBuffer>(size);
		}

		AU_CORE_LOG_ERROR("Unknown RendererAPI!");
		return nullptr;
	}

	std::shared_ptr<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t size) {
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			AU_CORE_LOG_ERROR("RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			AU_CORE_LOG_ERROR("RendererAPI::OpenGL is currently not supported!");
			return nullptr;
		case RendererAPI::API::Vulkan:
			return std::make_shared<VulkanIndexBuffer>(indices, size);
		}

		AU_CORE_LOG_ERROR("Unknown RendererAPI!");
		return nullptr;
	}

}