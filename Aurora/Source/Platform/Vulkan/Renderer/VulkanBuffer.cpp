#include "aupch.h"
#include "VulkanBuffer.h"

#include "VulkanContext.h"
#include "Aurora/Renderer/RenderCommand.h"
#include "../Utils/VulkanHelper.h"

namespace Aurora {
	// TODO: write the rest of the buffer handling
	
	VulkanVertexBuffer::VulkanVertexBuffer(uint32_t size): m_Buffer(nullptr), m_BufferMemory(nullptr) {
	}

	VulkanVertexBuffer::VulkanVertexBuffer(float* vertices, uint32_t size): m_Buffer(nullptr), m_BufferMemory(nullptr) {
		// VkDeviceSize bufferSize = sizeof(vertices[0]) * size;
		VkDeviceSize bufferSize = sizeof(float) * size;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		vkhelper::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
		
		VkDevice device = RenderCommand::GetContextAs<VulkanContext>()->GetDevice();

		if (device == VK_NULL_HANDLE) {
			AU_CORE_CRITICAL("DEVICE ERROR!");
		}
		
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

	VulkanVertexBuffer::~VulkanVertexBuffer() {
		VkDevice device = RenderCommand::GetContextAs<VulkanContext>()->GetDevice();
		vkDestroyBuffer(device, m_Buffer, nullptr);
		vkFreeMemory(device, m_BufferMemory, nullptr);
	}

	void VulkanVertexBuffer::Bind() const {  }

	void VulkanVertexBuffer::Unbind() const {  }

	void VulkanVertexBuffer::SetData(const void* data, uint32_t size) {
	}

	VulkanIndexBuffer::VulkanIndexBuffer(uint32_t count): m_Buffer(nullptr), m_BufferMemory(nullptr), m_Count(count) {
		// TODO
	}

	VulkanIndexBuffer::VulkanIndexBuffer(uint32_t* indices, uint32_t size): m_Buffer(nullptr), m_BufferMemory(nullptr),
	                                                                        m_Count(size) {
		VkDeviceSize bufferSize = sizeof(uint32_t) * size;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		vkhelper::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
		
		VkDevice device = RenderCommand::GetContextAs<VulkanContext>()->GetDevice();

		if (device == VK_NULL_HANDLE) {
			AU_CORE_CRITICAL("DEVICE ERROR!");
		}
		
		void* data;
		vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices, (size_t) bufferSize);
		vkUnmapMemory(device, stagingBufferMemory);

		vkhelper::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Buffer, m_BufferMemory);

		VkCommandPool& commandPool = RenderCommand::GetContextAs<VulkanContext>()->GetCommandPool();
		vkhelper::copyBuffer(stagingBuffer, m_Buffer, bufferSize, commandPool);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
	}

	VulkanIndexBuffer::~VulkanIndexBuffer() {
		VkDevice device = RenderCommand::GetContextAs<VulkanContext>()->GetDevice();
		vkDestroyBuffer(device, m_Buffer, nullptr);
		vkFreeMemory(device, m_BufferMemory, nullptr);
	}

	void VulkanIndexBuffer::Bind() const {
	}

	void VulkanIndexBuffer::Unbind() const {
	}

	void VulkanIndexBuffer::SetData(const void* data, uint32_t size) {
	}

}
