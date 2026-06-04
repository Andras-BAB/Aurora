#include "aupch.h"

#include "DirectX12GraphAllocator.h"

#include <format>

namespace Aurora {
	DirectX12GraphAllocator::DirectX12GraphAllocator(DirectX12Context* context, DirectX12TextureManager* textureManager)
		: m_Context(context), m_TextureManager(textureManager) {

	}

	DirectX12GraphAllocator::~DirectX12GraphAllocator() {
		ClearPool();
	}

	void DirectX12GraphAllocator::AcquireTexture(const GraphTextureDesc& desc, RenderGraphResourceRegistry::PhysicalResourceData& outData) {
		TexturePoolKey key = { .Width = desc.Width, .Height = desc.Height, .Format = desc.Format, .SampleCount = desc.SampleCount };

		// if there is a free texture, we use it (aliasing)
		auto it = m_FreeTextures.find(key);
		if (it != m_FreeTextures.end() && !it->second.empty()) {
			outData = it->second.back();
			it->second.pop_back();
			return;
		}

		auto device = m_Context->GetDevice();
		auto heapManager = m_Context->GetHeapManager();

		D3D12_RESOURCE_DESC d3dDesc = {};
		d3dDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		d3dDesc.Alignment = 0;
		d3dDesc.Width = desc.Width;
		d3dDesc.Height = desc.Height;
		d3dDesc.DepthOrArraySize = 1;
		d3dDesc.MipLevels = 1;
		d3dDesc.SampleDesc.Count = desc.SampleCount;
		d3dDesc.SampleDesc.Quality = desc.SampleQuality;
		d3dDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

		
		DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI_FORMAT resourceFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI_FORMAT srvFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		if (desc.Format == ImageFormat::RGBA16F) {
			dxgiFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
			resourceFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;
			srvFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;

		} else if (desc.Format == ImageFormat::DEPTH24_STENCIL8) {
			dxgiFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
			resourceFormat = DXGI_FORMAT_R24G8_TYPELESS;
			srvFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

		} else if (desc.Format == ImageFormat::DEPTH32F) {
			dxgiFormat = DXGI_FORMAT_D32_FLOAT;
			resourceFormat = DXGI_FORMAT_R32_TYPELESS;
			srvFormat = DXGI_FORMAT_R32_FLOAT;

		} else if (desc.Format == ImageFormat::DEPTH32F_STENCIL8 || desc.Format == ImageFormat::Depth) {
			dxgiFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
			resourceFormat = DXGI_FORMAT_R32G8X24_TYPELESS;
			srvFormat = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
		}
		d3dDesc.Format = resourceFormat;

		bool isDepth = (dxgiFormat == DXGI_FORMAT_D24_UNORM_S8_UINT ||
			dxgiFormat == DXGI_FORMAT_D32_FLOAT ||
			dxgiFormat == DXGI_FORMAT_D32_FLOAT_S8X24_UINT);
		if (isDepth) {
			d3dDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		} else {
			d3dDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		}

		D3D12_HEAP_PROPERTIES heapProps = {};
		heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_CLEAR_VALUE optClear = {};
		optClear.Format = dxgiFormat;
		if (isDepth) {
			//optClear.DepthStencil.Depth = 1.0f;
			optClear.DepthStencil.Depth = 0.0f;
			optClear.DepthStencil.Stencil = 0;
		} else {
			optClear.Color[0] = 0.0f;
			optClear.Color[1] = 0.0f;
			optClear.Color[2] = 0.0f;
			optClear.Color[3] = 1.0f;
		}

		ID3D12Resource* physicalResource = nullptr;
		HRESULT hr = device->CreateCommittedResource(
			&heapProps, D3D12_HEAP_FLAG_NONE, &d3dDesc,
			D3D12_RESOURCE_STATE_COMMON, &optClear,
			IID_PPV_ARGS(&physicalResource)
		);

		if (FAILED(hr)) {
			AU_CORE_ERROR("Error occured while allocating texture for the RenderGraph: {0}", MS::HrToString(hr));
			return;
		}

		outData.Resource = physicalResource;
		outData.CurrentState = D3D12_RESOURCE_STATE_COMMON;

		if (isDepth) {
			DescriptorRange dsvRange = heapManager->AllocateDSV(1);
			outData.DsvHandlePtr = dsvRange.cpuBase.handle.ptr;

			D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
			dsvDesc.Format = dxgiFormat;
			//dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			if (desc.SampleCount > 1) {
				dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
			} else {
				dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			}
			dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

			device->CreateDepthStencilView(physicalResource, &dsvDesc, dsvRange.cpuBase.handle);
		} else {
			DescriptorRange rtvRange = heapManager->AllocateRTV(1);
			outData.RtvHandlePtr = rtvRange.cpuBase.handle.ptr;
			device->CreateRenderTargetView(physicalResource, nullptr, rtvRange.cpuBase.handle);
		}

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = srvFormat;
		//srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		//srvDesc.Texture2D.MostDetailedMip = 0;
		//srvDesc.Texture2D.MipLevels = 1;

		if (desc.SampleCount > 1) {
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
		} else {
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = 1;
		}

		outData.BindlessHandle = m_TextureManager->CreateTextureSRV(physicalResource, srvDesc);

		m_AllocatedTextures.push_back({ key, outData });
	}

	void DirectX12GraphAllocator::ReleaseTexture(const GraphTextureDesc& desc, const RenderGraphResourceRegistry::PhysicalResourceData& data) {
		TexturePoolKey key = { .Width = desc.Width, .Height = desc.Height, .Format = desc.Format, .SampleCount = desc.SampleCount };
		m_FreeTextures[key].push_back(data);
	}

	void DirectX12GraphAllocator::AcquireBuffer(const GraphBufferDesc& desc, RenderGraphResourceRegistry::PhysicalResourceData& outData) {
		BufferPoolKey key = { desc.Size };

		auto it = m_FreeBuffers.find(key);
		if (it != m_FreeBuffers.end() && !it->second.empty()) {
			outData = it->second.back();
			it->second.pop_back();
			return;
		}

		auto context = m_Context;
		auto device = context->GetDevice();

		D3D12_HEAP_PROPERTIES heapProps = {};
		heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProps.CreationNodeMask = 1;
		heapProps.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC bufferDesc = {};
		bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		bufferDesc.Alignment = 0;
		bufferDesc.Width = desc.Size;
		bufferDesc.Height = 1;
		bufferDesc.DepthOrArraySize = 1;
		bufferDesc.MipLevels = 1;
		bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		bufferDesc.SampleDesc.Count = 1;
		bufferDesc.SampleDesc.Quality = 0;
		bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		bufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		ID3D12Resource* resource = nullptr;
		device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&resource)
		);

		outData.Resource = resource;
		outData.CurrentState = D3D12_RESOURCE_STATE_COMMON;

		outData.BindlessHandle = m_TextureManager->AllocateDescriptor();
		outData.BindlessUAVHandle = m_TextureManager->AllocateDescriptor();

		auto bindlessHeap = m_TextureManager->GetBindlessHeap();
		UINT descriptorSize = context->GetHeapManager()->GetCbvSrvUavIncrementSize();

		D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = bindlessHeap->GetCPUDescriptorHandleForHeapStart();
		srvHandle.ptr += static_cast<SIZE_T>(outData.BindlessHandle.Index) * descriptorSize;

		D3D12_CPU_DESCRIPTOR_HANDLE uavHandle = bindlessHeap->GetCPUDescriptorHandleForHeapStart();
		uavHandle.ptr += static_cast<SIZE_T>(outData.BindlessUAVHandle.Index) * descriptorSize;

		D3D12_SHADER_RESOURCE_VIEW_DESC srvViewDesc = {};
		srvViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvViewDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvViewDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvViewDesc.Buffer.FirstElement = 0;
		srvViewDesc.Buffer.NumElements = desc.Size / desc.ElementSize;
		srvViewDesc.Buffer.StructureByteStride = desc.ElementSize;
		srvViewDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		device->CreateShaderResourceView(resource, &srvViewDesc, srvHandle);

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavViewDesc = {};
		uavViewDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavViewDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavViewDesc.Buffer.FirstElement = 0;
		uavViewDesc.Buffer.NumElements = desc.Size / desc.ElementSize;
		uavViewDesc.Buffer.StructureByteStride = desc.ElementSize;
		uavViewDesc.Buffer.CounterOffsetInBytes = 0;
		uavViewDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
		device->CreateUnorderedAccessView(resource, nullptr, &uavViewDesc, uavHandle);

		m_AllocatedBuffers.push_back({ key, outData });
	}

	void DirectX12GraphAllocator::ReleaseBuffer(const GraphBufferDesc& desc, const RenderGraphResourceRegistry::PhysicalResourceData& data) {
		BufferPoolKey key = { desc.Size };
		m_FreeBuffers[key].push_back(data);
	}

	void DirectX12GraphAllocator::BeginFrame() {
		m_FreeTextures.clear();
		m_FreeBuffers.clear();

		// we add all to the free textures, so we can reuse them all
		for (const auto& alloc : m_AllocatedTextures) {
			m_FreeTextures[alloc.Key].push_back(alloc.Data);
		}
		for (const auto& alloc : m_AllocatedBuffers) {
			m_FreeBuffers[alloc.Key].push_back(alloc.Data);
		}
	}

	void DirectX12GraphAllocator::ClearPool() {
		for (auto& alloc : m_AllocatedTextures) {
			if (alloc.Data.Resource) {
				static_cast<ID3D12Resource*>(alloc.Data.Resource)->Release();
			}
			if (alloc.Data.BindlessHandle.IsValid()) {
				m_TextureManager->ReleaseTextureSRV(alloc.Data.BindlessHandle);
			}
		}
		m_AllocatedTextures.clear();
		m_FreeTextures.clear();

		for (auto& alloc : m_AllocatedBuffers) {
			if (alloc.Data.Resource) {
				static_cast<ID3D12Resource*>(alloc.Data.Resource)->Release();
			}
			if (alloc.Data.BindlessHandle.IsValid()) {
				m_TextureManager->ReleaseTextureSRV(alloc.Data.BindlessHandle);
			}
			if (alloc.Data.BindlessUAVHandle.IsValid()) {
				m_TextureManager->ReleaseTextureSRV(alloc.Data.BindlessUAVHandle);
			}
		}
		m_AllocatedBuffers.clear();
		m_FreeBuffers.clear();
	}

	void DirectX12GraphAllocator::UpdateFinalState(void* physicalResource, uint32_t finalState) {
		for (auto& alloc : m_AllocatedTextures) {
			if (alloc.Data.Resource == physicalResource) {
				alloc.Data.CurrentState = finalState;
				return;
			}
		}

		for (auto& alloc : m_AllocatedBuffers) {
			if (alloc.Data.Resource == physicalResource) {
				alloc.Data.CurrentState = finalState;
				return;
			}
		}
	}
}
