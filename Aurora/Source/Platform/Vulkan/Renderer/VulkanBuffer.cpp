#include "aupch.h"
#include "VulkanBuffer.h"

#include "VulkanContext.h"
#include "Aurora/Renderer/RenderCommand.h"
#include "../Utils/VulkanHelper.h"

namespace Aurora {
	VulkanVertexBuffer::VulkanVertexBuffer(uint32_t size): m_Buffer(nullptr), m_BufferMemory(nullptr) {
	}

	VulkanVertexBuffer::VulkanVertexBuffer(float* vertices, uint32_t size): m_Buffer(nullptr), m_BufferMemory(nullptr) {
		VkDeviceSize bufferSize = sizeof(vertices[0]) * size;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		vkhelper::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
		
		VkDevice& device = RenderCommand::GetContextAs<VulkanContext>()->GetDevice();
		
		void* data;
		vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices, (size_t) bufferSize);
		vkUnmapMemory(device, stagingBufferMemory);

		vkhelper::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Buffer, m_BufferMemory);

		VkCommandPool& commandPool = RenderCommand::GetContextAs<VulkanContext>()->GetCommandPool();
		vkhelper::copyBuffer(stagingBuffer, m_Buffer, bufferSize, commandPool);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
	}

	void VulkanVertexBuffer::Bind() const {  }

	void VulkanVertexBuffer::Unbind() const {  }

	void VulkanVertexBuffer::SetData(const void* data, uint32_t size) {
	}

	VulkanIndexBuffer::VulkanIndexBuffer(uint32_t* indices, uint32_t size): m_Buffer(nullptr), m_BufferMemory(nullptr),
	                                                                        m_Count(size) {
	}

	void VulkanIndexBuffer::Bind() const {
	}

	void VulkanIndexBuffer::Unbind() const {
	}
}
