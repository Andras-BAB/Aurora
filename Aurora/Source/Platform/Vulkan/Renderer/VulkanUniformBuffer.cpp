#include "aupch.h"
#include "Platform/Vulkan/Renderer/VulkanUniformBuffer.h"

#include "VulkanContext.h"
#include "VulkanRenderCommand.h"
#include "Platform/Vulkan/Utils/VulkanHelper.h"

namespace Aurora {
	VulkanUniformBuffer::VulkanUniformBuffer(uint32_t size, uint32_t binding) {
		m_Buffer = vkhelper::createBuffer(size,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			m_Buffer.Handle);

		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = binding;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT; // TODO: make it settable
		uboLayoutBinding.pImmutableSamplers = nullptr; // only for image sampling

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uboLayoutBinding;

		VulkanContext* context = VulkanRenderCommand::GetContext();
		
		VK_CHECK(vkCreateDescriptorSetLayout(context->GetDevice(), &layoutInfo, nullptr, &m_DescriptorSetLayout));
	}

	VulkanUniformBuffer::~VulkanUniformBuffer() {
		VkDevice device = VulkanRenderCommand::GetContext()->GetDevice();

		vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr);
		
		vkDestroyBuffer(device, m_Buffer.Handle, nullptr);
		vkFreeMemory(device, m_Buffer.Memory, nullptr);
	}

	void VulkanUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset) {
	}
}
