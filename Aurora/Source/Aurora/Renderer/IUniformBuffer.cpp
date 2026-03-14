#include "aupch.h"

#include "Aurora/Renderer/IUniformBuffer.h"

#include "Aurora/Renderer/Renderer.h"
#include "Aurora/Renderer/RendererAPI.h"
#include "Aurora/Core/Assert.h"

namespace Aurora {

	std::shared_ptr<IUniformBuffer> IUniformBuffer::Create(uint32_t size, uint32_t binding) {
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			AU_CORE_ASSERT(false, "Unknown Render API!");
			return nullptr;
		case RendererAPI::API::DirectX12:
			AU_CORE_ASSERT(false, "Renderer API DirectX12 is currently not implemented!");
			return nullptr;
		case RendererAPI::API::Vulkan:
			AU_CORE_ASSERT(false, "Renderer API Vulkan is currently not supported!");
			return nullptr;
		case RendererAPI::API::DirectX11:
			AU_CORE_ASSERT(false, "Renderer API DirectX11 is currently not supported!");
			return nullptr;
		}

		AU_CORE_ASSERT(false, "Unknown Renderer API!");
		return nullptr;
	}
	
}
