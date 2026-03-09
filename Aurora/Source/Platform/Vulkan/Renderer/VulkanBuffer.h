#pragma once
#include "tiny_gltf.h"
#include "Aurora/Renderer/Buffer.h"

#include <vulkan/vulkan.h>

namespace Aurora {

	struct VulkanBuffer {
		VkBuffer Handle = VK_NULL_HANDLE;
		VkDeviceMemory Memory = VK_NULL_HANDLE;
	};
	
	class VulkanVertexBuffer : public VertexBuffer {
	public:
		VulkanVertexBuffer(uint32_t size);
		VulkanVertexBuffer(float* vertices, uint32_t size);
		virtual ~VulkanVertexBuffer() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetData(const void* data, uint32_t size) override;

		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
	public:
		VulkanBuffer m_Buffer;
		BufferLayout m_Layout;
	};

	class VulkanIndexBuffer : public IndexBuffer {
	public:
		VulkanIndexBuffer(uint32_t count);
		VulkanIndexBuffer(uint32_t* indices, uint32_t size);
		virtual ~VulkanIndexBuffer() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetData(const void* data, uint32_t size) override;

		virtual uint32_t GetCount() const override { return m_Count; }
	public:
		VulkanBuffer m_Buffer;
		uint32_t m_Count;
	};
	
}
