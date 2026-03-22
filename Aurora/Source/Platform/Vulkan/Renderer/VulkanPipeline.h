#pragma once

#include "Aurora/Renderer/Pipeline.h"
#include "VertexLayoutDescriptor.h"
//#include <vulkan/vulkan.h>

namespace Aurora {

	class VulkanPipeline : public Pipeline {
	public:
		VulkanPipeline(const PipelineSpecification& spec);
		~VulkanPipeline() override;

		void Bind() override;
		void Unbind() override;

		const PipelineSpecification& GetSpecification() const override { return m_Specification; }
		std::shared_ptr<Shader> GetShader(ShaderStage stage) const override;

		VkPipeline GetVulkanPipeline() const { return m_Pipeline; }
		VkPipelineLayout GetVulkanPipelineLayout() const { return m_PipelineLayout; }

	private:
		void CreatePipeline();
		void Cleanup();

		static VkPrimitiveTopology ToVulkanTopology(PrimitiveTopology topology);
		static VkCullModeFlags ToVulkanCullMode(CullMode cullMode);
		static VkFrontFace ToVulkanFrontFace(FrontFace frontFace);

	private:
		PipelineSpecification m_Specification;
		VkDevice m_Device = VK_NULL_HANDLE;
		VkPipeline m_Pipeline = VK_NULL_HANDLE;
		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;

		std::vector<VkDescriptorSetLayoutBinding> m_DescriptorSetLayoutBinding;
		VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;

		std::vector<VkPushConstantRange> m_PushConstantRanges;
		
		std::unique_ptr<VertexLayoutDescriptor> m_VertexLayoutDescriptor;
	};

}
