#include "aupch.h"
#include "Pipeline.h"

#include "Renderer.h"
#include "Aurora/Core/Assert.h"
#include "Platform/Vulkan/Renderer/VulkanPipeline.h"

namespace Aurora {

	std::shared_ptr<Pipeline> Pipeline::Create(const PipelineSpecification& spec) {
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			AU_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::Vulkan:
			return std::make_shared<VulkanPipeline>(spec);
		}

		AU_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}
