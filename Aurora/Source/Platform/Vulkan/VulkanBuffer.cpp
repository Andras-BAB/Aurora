#include "aupch.h"
#include "Platform/Vulkan/VulkanBuffer.h"

namespace Aurora {
	VulkanVertexBuffer::VulkanVertexBuffer(uint32_t size) {
	}

	VulkanVertexBuffer::VulkanVertexBuffer(float* vertices, uint32_t size) {
	}

	VulkanVertexBuffer::~VulkanVertexBuffer() {
	}

	void VulkanVertexBuffer::Bind() const {  }

	void VulkanVertexBuffer::Unbind() const {  }

	void VulkanVertexBuffer::SetData(const void* data, uint32_t size) {
	}

	VulkanIndexBuffer::VulkanIndexBuffer(uint32_t* indices, uint32_t size) {  }

	VulkanIndexBuffer::~VulkanIndexBuffer() {
	}

	void VulkanIndexBuffer::Bind() const {
	}

	void VulkanIndexBuffer::Unbind() const {
	}
}
