#pragma once

#include "Aurora/Renderer/Buffer.h"

namespace Aurora {
	class DirectX12StructuredBuffer : public StructuredBuffer {
	public:
		DirectX12StructuredBuffer(uint32_t elementCount, uint32_t elementSize);
		~DirectX12StructuredBuffer() override = default;

		TextureHandle GetSRV() const override { return m_SRVHandle; }
		TextureHandle GetUAV() const override { return m_UAVHandle; }

		uint32_t GetElementCount() const override { return m_ElementCount; }
		uint32_t GetElementSize() const override { return m_ElementSize; }

		uint32_t GetCurrentState() const override { return m_CurrentState; }
		void SetCurrentState(uint32_t state) override { m_CurrentState = state; }

		void* GetRawResource() const override { return (void*)m_Resource.Get(); }

	private:
		void CreateViews();

	private:
		MS::ComPtr<ID3D12Resource> m_Resource;
		uint32_t m_ElementCount;
		uint32_t m_ElementSize;

		TextureHandle m_SRVHandle;
		TextureHandle m_UAVHandle;

		uint32_t m_CurrentState = D3D12_RESOURCE_STATE_COMMON;
	};
}
