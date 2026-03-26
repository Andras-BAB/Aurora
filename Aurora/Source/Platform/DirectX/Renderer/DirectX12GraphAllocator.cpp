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
		TexturePoolKey key = { .Width = desc.Width, .Height = desc.Height, .Format = desc.Format };

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
		d3dDesc.SampleDesc.Count = 1;
		d3dDesc.SampleDesc.Quality = 0;
		d3dDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;


		DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		if (desc.Format == ImageFormat::DEPTH24_STENCIL8) {
			dxgiFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		} else if (desc.Format == ImageFormat::DEPTH32F) {
			dxgiFormat = DXGI_FORMAT_D32_FLOAT;
		} else if (desc.Format == ImageFormat::DEPTH32F_STENCIL8 || desc.Format == ImageFormat::Depth) {
			dxgiFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
		}
		d3dDesc.Format = dxgiFormat;

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
			device->CreateDepthStencilView(physicalResource, nullptr, dsvRange.cpuBase.handle);
		} else {
			DescriptorRange rtvRange = heapManager->AllocateRTV(1);
			outData.RtvHandlePtr = rtvRange.cpuBase.handle.ptr;
			device->CreateRenderTargetView(physicalResource, nullptr, rtvRange.cpuBase.handle);
		}

		if (!isDepth) {
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Format = dxgiFormat;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = 1;

			outData.BindlessHandle = m_TextureManager->CreateTextureSRV(physicalResource, srvDesc);
		}

		m_AllocatedTextures.push_back({ key, outData });
	}

	void DirectX12GraphAllocator::ReleaseTexture(const GraphTextureDesc& desc, const RenderGraphResourceRegistry::PhysicalResourceData& data) {
		TexturePoolKey key = { .Width = desc.Width, .Height = desc.Height, .Format = desc.Format };
		m_FreeTextures[key].push_back(data);
	}

	void DirectX12GraphAllocator::AcquireBuffer(const GraphBufferDesc& desc,
		RenderGraphResourceRegistry::PhysicalResourceData& outData) {
	}

	void DirectX12GraphAllocator::ReleaseBuffer(const GraphBufferDesc& desc,
		const RenderGraphResourceRegistry::PhysicalResourceData& data) {
	}

	void DirectX12GraphAllocator::BeginFrame() {
		m_FreeTextures.clear();

		// we add all to the free textures, so we can reuse them all
		for (const auto& alloc : m_AllocatedTextures) {
			m_FreeTextures[alloc.Key].push_back(alloc.Data);
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
	}

	void DirectX12GraphAllocator::UpdateFinalState(void* physicalResource, uint32_t finalState) {
		for (auto& alloc : m_AllocatedTextures) {
			if (alloc.Data.Resource == physicalResource) {
				alloc.Data.CurrentState = finalState;
				return;
			}
		}
	}
}
