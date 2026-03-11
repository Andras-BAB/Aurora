#include "aupch.h"
#include "Aurora/Renderer/UniformBuffer.h"

#include "Aurora/Renderer/Renderer.h"
#include "Aurora/Renderer/RendererAPI.h"
#include "Aurora/Core/Assert.h"
#include "Platform/Vulkan/Renderer/VulkanUniformBuffer.h"

namespace Aurora {

	std::shared_ptr<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding) {
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			AU_CORE_ASSERT(false, "Unknown Render API!");
			return nullptr;
		case RendererAPI::API::DirectX12:
			AU_CORE_ASSERT(false, "Render API DirectX12 is currently not supported!");
			return nullptr;
		case RendererAPI::API::Vulkan:
			return std::make_shared<VulkanUniformBuffer>(size, binding);
		case RendererAPI::API::OpenGL:
			AU_CORE_ASSERT(false, "Render API OpenGL is WIP!");
			return nullptr;
		case RendererAPI::API::DirectX11:
			AU_CORE_ASSERT(false, "Render API DirectX11 is currently not supported!");
			return nullptr;
		}

		AU_CORE_ASSERT(false, "Unknown Render API!");
		return nullptr;
	}
	
}
