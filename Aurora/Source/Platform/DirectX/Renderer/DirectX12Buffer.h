#pragma once
#include "DirectX12Context.h"
#include "Aurora/Renderer/Buffer.h"
#include "Platform/DirectX/Utils/MSUtils.h"

namespace Aurora {
	
	class DirectX12VertexBuffer : public VertexBuffer {
	public:
		DirectX12VertexBuffer(uint32_t size);
		DirectX12VertexBuffer(float* vertices, uint32_t size);
		~DirectX12VertexBuffer() override = default;

		void SetData(const void* data, uint32_t size) override;
		
		void Bind() const override;
		void Unbind() const override;

		void SetLayout(const BufferLayout& layout) override;
		const BufferLayout& GetLayout() const override;

	private:
		BufferLayout m_BufferLayout;
		MS::ComPtr<ID3DBlob> m_CPUBuffer;
		MS::ComPtr<ID3D12Resource> m_GPUBuffer;
		MS::ComPtr<ID3D12Resource> m_UploadBuffer;
		// Caching
		DirectX12Context* m_GraphicsContext;
		D3D12_VERTEX_BUFFER_VIEW m_BufferView;
	};

	class DirectX12IndexBuffer : public IndexBuffer {
	public:
		DirectX12IndexBuffer(uint32_t size);
		DirectX12IndexBuffer(uint32_t* indices, uint32_t count);
		~DirectX12IndexBuffer() override = default;

		void Bind() const override;
		void Unbind() const override;

		uint32_t GetCount() const override;
		void SetData(const void* data, uint32_t size) override;

	private:
		uint32_t m_Count;
		MS::ComPtr<ID3DBlob> m_CPUBuffer;
		MS::ComPtr<ID3D12Resource> m_GPUBuffer;
		MS::ComPtr<ID3D12Resource> m_UploadBuffer;
		// Cache
		DirectX12Context* m_GraphicsContext;
		D3D12_INDEX_BUFFER_VIEW m_BufferView;
	};
	
}
