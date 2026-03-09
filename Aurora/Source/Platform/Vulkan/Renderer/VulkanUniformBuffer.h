#pragma once
#include "VulkanBuffer.h"
#include "Aurora/Renderer/Buffer.h"
#include "Aurora/Renderer/UniformBuffer.h"

#include "vulkan/vulkan.h"

namespace Aurora {
	
	class VulkanUniformBuffer : public UniformBuffer {
	public:
		VulkanUniformBuffer(uint32_t size, uint32_t binding);
		~VulkanUniformBuffer() override;
		
		void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;

	private:
		VulkanBuffer m_Buffer;
		void* m_MappedData = nullptr;
		// std::vector<void*> m_MappedData;
		BufferLayout m_Layout;

		VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
	};
	
}
