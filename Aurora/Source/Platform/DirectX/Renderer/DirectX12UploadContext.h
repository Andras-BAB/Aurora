#pragma once
#include "Platform/DirectX/Utils/MSUtils.h"

#include <d3d12.h>
#include <vector>

#include "D3D12MemAlloc.h"

namespace Aurora {

	class DirectX12Context;

	class DirectX12UploadContext {
	public:
		void Init(DirectX12Context* context);

		void UploadTexture(ID3D12Resource* dest, const void* data, uint32_t rowPitch, uint32_t slicePitch,
			D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter);

		void UploadBuffer(ID3D12Resource* dest, const void* data, uint64_t size,
			D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter);

		void ProcessCompletedUploads();

	private:
		struct PendingUpload {
			MS::ComPtr<ID3D12CommandAllocator> CommandAllocator;
			MS::ComPtr<ID3D12GraphicsCommandList> CommandList;
			MS::ComPtr<D3D12MA::Allocation> StagingAllocation;
			MS::ComPtr<ID3D12Resource> StagingResource;
			UINT64 FenceValue = 0;
		};

		PendingUpload BeginUpload(uint64_t stagingSize);
		void SubmitUpload(PendingUpload&& pending);

		DirectX12Context* m_Context = nullptr;
		std::vector<PendingUpload> m_PendingUploads;
	};
}
