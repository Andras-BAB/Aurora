#include "aupch.h"
#include "VulkanPipeline.h"

#include "VulkanRenderCommand.h"
#include "VulkanRenderPass.h"
#include "VulkanShader.h"
#include "Aurora/Renderer/RenderCommand.h"
#include "Aurora/Core/Log.h"
#include "Aurora/Core/Assert.h"
#include "Platform/Vulkan/Utils/VulkanHelper.h"

namespace Aurora {
	VulkanPipeline::VulkanPipeline(const PipelineSpecification& spec) 
		: m_Specification(spec) {
		AU_CORE_ASSERT(m_Specification.shader, "Shader cannot be null!");
		AU_CORE_ASSERT(m_Specification.renderPass, "RenderPass cannot be null!");

		m_Device = VulkanRenderCommand::GetDevice();

		// Create vertex layout descriptor
		m_VertexLayoutDescriptor = std::make_unique<VertexLayoutDescriptor>(0, m_Specification.vertexLayout);

		CreatePipeline();
	}

	VulkanPipeline::~VulkanPipeline() {
		Cleanup();
	}

	void VulkanPipeline::Bind() {
		// Get current command buffer from context
		VulkanContext* context = RenderCommand::GetContextAs<VulkanContext>();
		VkCommandBuffer commandBuffer = context->GetCurrentCommandBuffer();

		if (commandBuffer != VK_NULL_HANDLE && m_Pipeline != VK_NULL_HANDLE) {
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
		}
	}

	void VulkanPipeline::Unbind() {
		// Vulkan-ban nincs explicit unbind, automatikusan történik a következő bind-nál
	}

	std::shared_ptr<Shader> VulkanPipeline::GetShader(const ShaderStage stage) const {
		return m_Specification.shader;
	}

	void VulkanPipeline::CreatePipeline() {
		// Get Vulkan shader
		auto vulkanShader = std::static_pointer_cast<VulkanShader>(m_Specification.shader);
		AU_CORE_ASSERT(vulkanShader, "Shader must be a VulkanShader!");
		
		// Get Vulkan render pass
		auto vulkanRenderPass = std::static_pointer_cast<VulkanRenderPass>(m_Specification.renderPass);
		AU_CORE_ASSERT(vulkanRenderPass, "RenderPass must be a VulkanRenderPass!");

		// Shader stages
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

		// Vertex shader
		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vulkanShader->GetVertexHandle();
		vertShaderStageInfo.pName = "main";
		shaderStages.push_back(vertShaderStageInfo);
		

		// Fragment shader
		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = vulkanShader->GetFragmentHandle();
		fragShaderStageInfo.pName = "main";
		shaderStages.push_back(fragShaderStageInfo);
		

		// Vertex input state
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		VkVertexInputBindingDescription bindingDescription = m_VertexLayoutDescriptor->GetBindingDescription();
		const auto& attributeDescriptions = m_VertexLayoutDescriptor->GetAttributeDescriptions();

		if (!m_Specification.vertexLayout.GetElements().empty()) {
			vertexInputInfo.vertexBindingDescriptionCount = 1;
			vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
			vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
			vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
		} else {
			vertexInputInfo.vertexBindingDescriptionCount = 0;
			vertexInputInfo.vertexAttributeDescriptionCount = 0;
		}

		// Input assembly state
		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = ToVulkanTopology(m_Specification.topology);
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		// Viewport state (dynamic)
		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		// Rasterization state
		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = m_Specification.wireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = m_Specification.lineWidth;
		rasterizer.cullMode = ToVulkanCullMode(m_Specification.cullMode);
		rasterizer.frontFace = ToVulkanFrontFace(m_Specification.frontFace);
		rasterizer.depthBiasEnable = VK_FALSE;

		// Multisampling
		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		// Depth/stencil state
		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = m_Specification.depthTest ? VK_TRUE : VK_FALSE;
		depthStencil.depthWriteEnable = m_Specification.depthWrite ? VK_TRUE : VK_FALSE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f;
		depthStencil.maxDepthBounds = 1.0f;
		depthStencil.stencilTestEnable = VK_FALSE;

		// Color blending
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | 
											  VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = m_Specification.blendEnable ? VK_TRUE : VK_FALSE;

		if (m_Specification.blendEnable) {
			colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
			colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
		}

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		// Dynamic state
		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		if (m_Specification.wireframe) {
			dynamicStates.push_back(VK_DYNAMIC_STATE_LINE_WIDTH);
		}

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		// Pipeline layout
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

		// TODO: create descriptorSetLayouts outside
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;		// for texture samplers

		m_DescriptorSetLayoutBinding.push_back(uboLayoutBinding);
		
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = m_DescriptorSetLayoutBinding.size();
		layoutInfo.pBindings = m_DescriptorSetLayoutBinding.data();

		vkCreateDescriptorSetLayout(m_Device, &layoutInfo, nullptr, &m_DescriptorSetLayout);
		
		// Get descriptor set layouts
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &m_DescriptorSetLayout;

		// create push constant ranges TODO: get it from outside

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(uint32_t);
		
		pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(m_PushConstantRanges.size());
		pipelineLayoutInfo.pPushConstantRanges = m_PushConstantRanges.data();

		VK_CHECK(vkCreatePipelineLayout(m_Device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout));

		// Graphics pipeline
		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = m_PipelineLayout;
		pipelineInfo.renderPass = vulkanRenderPass->GetVulkanRenderPass();
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		VK_CHECK(vkCreateGraphicsPipelines(m_Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline));

		AU_CORE_INFO("VulkanPipeline created successfully");
	}

	void VulkanPipeline::Cleanup() {
		if (m_Pipeline != VK_NULL_HANDLE && m_Device != VK_NULL_HANDLE) {
			vkDestroyPipeline(m_Device, m_Pipeline, nullptr);
			m_Pipeline = VK_NULL_HANDLE;
		}

		if (m_PipelineLayout != VK_NULL_HANDLE && m_Device != VK_NULL_HANDLE) {
			vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);
			m_PipelineLayout = VK_NULL_HANDLE;
		}
	}
	
	VkPrimitiveTopology VulkanPipeline::ToVulkanTopology(PrimitiveTopology topology) {
		switch (topology) {
		case PrimitiveTopology::Triangles:
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		case PrimitiveTopology::Lines:
			return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		case PrimitiveTopology::Points:
			return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		default:
			AU_CORE_ERROR("Unknown PrimitiveTopology!");
			return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		}
	}

	VkCullModeFlags VulkanPipeline::ToVulkanCullMode(CullMode cullMode) {
		switch (cullMode) {
		case CullMode::None:
			return VK_CULL_MODE_NONE;
		case CullMode::Front:
			return VK_CULL_MODE_FRONT_BIT;
		case CullMode::Back:
			return VK_CULL_MODE_BACK_BIT;
		case CullMode::FrontAndBack:
			return VK_CULL_MODE_FRONT_AND_BACK;
		default:
			AU_CORE_ERROR("Unknown CullMode!");
			return VK_CULL_MODE_BACK_BIT;
		}
	}

	VkFrontFace VulkanPipeline::ToVulkanFrontFace(FrontFace frontFace) {
		switch (frontFace) {
		case FrontFace::Clockwise:
			return VK_FRONT_FACE_CLOCKWISE;
		case FrontFace::CounterClockwise:
			return VK_FRONT_FACE_COUNTER_CLOCKWISE;
		default:
			AU_CORE_ERROR("Unknown FrontFace!");
			return VK_FRONT_FACE_COUNTER_CLOCKWISE;
		}
	}
}
