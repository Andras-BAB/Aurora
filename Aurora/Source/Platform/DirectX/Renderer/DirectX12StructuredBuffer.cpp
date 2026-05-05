#include "aupch.h"

#include "DirectX12StructuredBuffer.h"
#include "DirectX12RenderCommand.h"

namespace Aurora {
	DirectX12StructuredBuffer::DirectX12StructuredBuffer(uint32_t elementCount, uint32_t elementSize) 
	: m_ElementCount(elementCount), m_ElementSize(elementSize)  {

		auto context = DirectX12RenderCommand::GetContext();
		auto device = context->GetDevice();

		uint64_t bufferSize = static_cast<uint64_t>(elementCount) * elementSize;

		D3D12_HEAP_PROPERTIES props = {};
		props.Type = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_RESOURCE_DESC bufferDesc = {};
		bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		bufferDesc.Alignment = 0;
		bufferDesc.Width = bufferSize;
		bufferDesc.Height = 1;
		bufferDesc.DepthOrArraySize = 1;
		bufferDesc.MipLevels = 1;
		bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		bufferDesc.SampleDesc.Count = 1;
		bufferDesc.SampleDesc.Quality = 0;
		bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		bufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		device->CreateCommittedResource(
			&props,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&m_Resource)
		);

		CreateViews();
	}

	void DirectX12StructuredBuffer::CreateViews() {
		auto context = RenderCommand::GetContextAs<DirectX12Context>();
		auto device = context->GetDevice();
		auto textureManager = RenderCommand::GetRendererAPIAs<DirectX12RendererAPI>()->GetTextureManager();

		m_SRVHandle = textureManager->AllocateDescriptor();
		m_UAVHandle = textureManager->AllocateDescriptor();

		auto bindlessHeap = textureManager->GetBindlessHeap();
		UINT descriptorSize = context->GetHeapManager()->GetCbvSrvUavIncrementSize();

		D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = bindlessHeap->GetCPUDescriptorHandleForHeapStart();
		srvHandle.ptr += static_cast<SIZE_T>(m_SRVHandle.Index) * descriptorSize;

		D3D12_CPU_DESCRIPTOR_HANDLE uavHandle = bindlessHeap->GetCPUDescriptorHandleForHeapStart();
		uavHandle.ptr += static_cast<SIZE_T>(m_UAVHandle.Index) * descriptorSize;

		// SRV
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = m_ElementCount;
		srvDesc.Buffer.StructureByteStride = m_ElementSize;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

		device->CreateShaderResourceView(m_Resource.Get(), &srvDesc, srvHandle);

		// UAV
		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = m_ElementCount;
		uavDesc.Buffer.StructureByteStride = m_ElementSize;
		uavDesc.Buffer.CounterOffsetInBytes = 0;
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		device->CreateUnorderedAccessView(m_Resource.Get(), nullptr, &uavDesc, uavHandle);
	}
}
