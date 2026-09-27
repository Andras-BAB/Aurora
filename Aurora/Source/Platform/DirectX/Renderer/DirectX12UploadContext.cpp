#include "aupch.h"
#include "DirectX12UploadContext.h"

#include "DirectX12Context.h"
#include "Platform/DirectX/Utils/d3dUtil.h"

namespace Aurora {

	void DirectX12UploadContext::Init(DirectX12Context* context) {
		m_Context = context;
	}

	DirectX12UploadContext::PendingUpload DirectX12UploadContext::BeginUpload(uint64_t stagingSize) {
		PendingUpload pending;

		auto* device = m_Context->GetDevice();
		auto* allocator = m_Context->GetAllocator();

		D3D12_RESOURCE_DESC bufferDesc = {
			D3D12_RESOURCE_DIMENSION_BUFFER, 0, stagingSize, 1, 1, 1,
			DXGI_FORMAT_UNKNOWN, { .Count = 1, .Quality = 0 }, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_NONE
		};

		D3D12MA::ALLOCATION_DESC uploadAllocDesc = {};
		uploadAllocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

		ThrowOnFail(allocator->CreateResource(&uploadAllocDesc, &bufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, pending.StagingAllocation.GetAddressOf(), IID_PPV_ARGS(&pending.StagingResource)));

		ThrowOnFail(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pending.CommandAllocator)));
		ThrowOnFail(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pending.CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&pending.CommandList)));

		return pending;
	}

	void DirectX12UploadContext::SubmitUpload(PendingUpload&& pending) {
		ThrowOnFail(pending.CommandList->Close());

		ID3D12CommandList* cmds[] = { pending.CommandList.Get() };
		m_Context->GetCommandQueue()->ExecuteCommandLists(1, cmds);

		pending.FenceValue = m_Context->GetNextFenceValue();
		m_PendingUploads.push_back(std::move(pending));
	}

	void DirectX12UploadContext::UploadTexture(ID3D12Resource* dest, const void* data, uint32_t rowPitch, uint32_t slicePitch,
		D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter) {

		UINT64 uploadBufferSize = d3dUtil::GetRequiredIntermediateSize(dest, 0, 1);
		PendingUpload pending = BeginUpload(uploadBufferSize);

		D3D12_SUBRESOURCE_DATA subresourceData = {};
		subresourceData.pData = data;
		subresourceData.RowPitch = rowPitch;
		subresourceData.SlicePitch = slicePitch;

		if (stateBefore != D3D12_RESOURCE_STATE_COPY_DEST) {
			D3D12_RESOURCE_BARRIER preBarrier = {};
			preBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			preBarrier.Transition.pResource = dest;
			preBarrier.Transition.StateBefore = stateBefore;
			preBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
			pending.CommandList->ResourceBarrier(1, &preBarrier);
		}

		d3dUtil::UpdateSubresources(pending.CommandList.Get(), dest, pending.StagingResource.Get(), 0, 0, 1, &subresourceData);

		if (stateAfter != D3D12_RESOURCE_STATE_COPY_DEST) {
			D3D12_RESOURCE_BARRIER postBarrier = {};
			postBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			postBarrier.Transition.pResource = dest;
			postBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			postBarrier.Transition.StateAfter = stateAfter;
			pending.CommandList->ResourceBarrier(1, &postBarrier);
		}

		SubmitUpload(std::move(pending));
	}

	void DirectX12UploadContext::UploadBuffer(ID3D12Resource* dest, const void* data, uint64_t size,
		D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter) {

		PendingUpload pending = BeginUpload(size);

		void* mapped = nullptr;
		ThrowOnFail(pending.StagingResource->Map(0, nullptr, &mapped));
		memcpy(mapped, data, size);
		pending.StagingResource->Unmap(0, nullptr);

		if (stateBefore != D3D12_RESOURCE_STATE_COPY_DEST) {
			D3D12_RESOURCE_BARRIER preBarrier = {};
			preBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			preBarrier.Transition.pResource = dest;
			preBarrier.Transition.StateBefore = stateBefore;
			preBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
			pending.CommandList->ResourceBarrier(1, &preBarrier);
		}

		pending.CommandList->CopyBufferRegion(dest, 0, pending.StagingResource.Get(), 0, size);

		if (stateAfter != D3D12_RESOURCE_STATE_COPY_DEST) {
			D3D12_RESOURCE_BARRIER postBarrier = {};
			postBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			postBarrier.Transition.pResource = dest;
			postBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			postBarrier.Transition.StateAfter = stateAfter;
			pending.CommandList->ResourceBarrier(1, &postBarrier);
		}

		SubmitUpload(std::move(pending));
	}

	void DirectX12UploadContext::ProcessCompletedUploads() {
		UINT64 completed = m_Context->GetCompletedFenceValue();

		std::erase_if(m_PendingUploads, [completed](const PendingUpload& pending) {
			return pending.FenceValue <= completed;
		});
	}
}
