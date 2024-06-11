#pragma once
#include <vulkan/vulkan.h>

#include "Renderer/Buffer.h"

namespace Aurora {
	class VulkanVertexBuffer : public VertexBuffer {
	public:
		VulkanVertexBuffer(uint32_t size);
		VulkanVertexBuffer(float* vertices, uint32_t size);
		virtual ~VulkanVertexBuffer();

		virtual void Bind() const;
		virtual void Unbind() const;

		virtual void SetData(const void* data, uint32_t size) override;

		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
	private:
		VkBuffer m_Buffer;
		VkDeviceMemory m_Memory;
		BufferLayout m_Layout;
	};

	class VulkanIndexBuffer : public IndexBuffer {
	public:
		VulkanIndexBuffer(uint32_t* indices, uint32_t size);
		virtual ~VulkanIndexBuffer();

		virtual void Bind() const;
		virtual void Unbind() const;

		virtual uint32_t GetCount() const { return m_Count; }
	private:
		VkBuffer m_Buffer;
		VkDeviceMemory m_Memory;
		uint32_t m_Count;
	};
}
