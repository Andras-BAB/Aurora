#include "aupch.h"
#include "Platform/DirectX/Renderer/FrameResource.h"

namespace Aurora {

	FrameResource::FrameResource(ID3D12Device* device) {
		ThrowOnFail(device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(CmdListAlloc.GetAddressOf())));

	}

	void FrameResource::Init(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount) {
		PassCB = std::make_unique<UploadBuffer<PassConstants>>(device, passCount, true);
		MaterialCB = std::make_unique<UploadBuffer<MaterialConstants>>(device, materialCount, true);
		ObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(device, objectCount, true);
	}

	FrameSync::FrameSync(ID3D12Device* device) {
		ThrowOnFail(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(CommandAllocator.GetAddressOf())));
	}

	void FrameRenderData::Init(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount) {
		PassCB = std::make_unique<UploadBuffer<PassConstants>>(device, passCount, true);
		MaterialCB = std::make_unique<UploadBuffer<MaterialConstants>>(device, materialCount, true);
		ObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(device, objectCount, true);
	}

	void FrameRenderData::Reset() {
		StagingBuffers.clear();
	}

	void FrameRenderData::ResizeCBs(ID3D12Device* device, uint32_t objCount, uint32_t matCount) {
		ObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(device, objCount, true);
		MaterialCB = std::make_unique<UploadBuffer<MaterialConstants>>(device, matCount, true);
	}
}
