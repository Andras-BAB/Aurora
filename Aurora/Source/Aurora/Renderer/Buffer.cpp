#include "aupch.h"
#include "Aurora/Renderer/Buffer.h"

#include "Aurora/Renderer/Renderer.h"
#include "Platform/DirectX/Renderer/DirectX12Buffer.h"

namespace Aurora {

	std::shared_ptr<VertexBuffer> VertexBuffer::Create(uint32_t size) {
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			AU_CORE_ERROR("RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::Vulkan:
			AU_CORE_ERROR("RendererAPI::Vulkan is currently not supported!");
			return nullptr;
		case RendererAPI::API::DirectX12:
			return std::make_shared<DirectX12VertexBuffer>(size);
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
		case RendererAPI::API::Vulkan:
			AU_CORE_ERROR("RendererAPI::Vulkan is currently not supported!");
			return nullptr;
		case RendererAPI::API::DirectX12:
			return std::make_shared<DirectX12VertexBuffer>(vertices, size);
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
		case RendererAPI::API::Vulkan:
			AU_CORE_ERROR("RendererAPI::Vulkan is currently not supported!");
			return nullptr;
		case RendererAPI::API::DirectX12:
			return std::make_shared<DirectX12IndexBuffer>(size);
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
		case RendererAPI::API::Vulkan:
			AU_CORE_ERROR("RendererAPI::Vulkan is currently not supported!");
			return nullptr;
		case RendererAPI::API::DirectX12:
			return std::make_shared<DirectX12IndexBuffer>(indices, size);
		case RendererAPI::API::DirectX11:
			AU_CORE_ERROR("RendererAPI::DirectX11 is currently not supported!");
			return nullptr;
		}

		AU_CORE_ERROR("Unknown RendererAPI!");
		return nullptr;
	}

}
