#include "aupch.h"

#include "DirectX12Texture.h"

#include <stb_image.h>

#include "DirectX12RenderCommand.h"
#include "Aurora/Core/Application.h"

namespace Aurora {
	DirectX12Texture2D::DirectX12Texture2D(const TextureSpecification& specification) {
	}

	DirectX12Texture2D::DirectX12Texture2D(const std::string& path) : m_Path(path) {
		auto* context = DirectX12RenderCommand::GetContext();
		auto* device = context->GetDevice();
		//auto* cmdList = context->GetCommandList();
		auto* queue = context->GetCommandQueue();

		MS::ComPtr<ID3D12CommandAllocator> tempAlloc;
		device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&tempAlloc));

		MS::ComPtr<ID3D12GraphicsCommandList> tempCmdList;
		device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, tempAlloc.Get(), nullptr, IID_PPV_ARGS(&tempCmdList));

		int width, height, channels;
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 4);
		if (!data) {
			AU_CORE_ERROR("Failed to load texture: {0}", path);
			return;
		}
		m_Width = width;
		m_Height = height;

		D3D12_RESOURCE_DESC texDesc = {};
		texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		texDesc.Alignment = 0;
		texDesc.Width = m_Width;
		texDesc.Height = m_Height;
		texDesc.DepthOrArraySize = 1;
		texDesc.MipLevels = 1;
		texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		D3D12_HEAP_PROPERTIES defaultHeap = { D3D12_HEAP_TYPE_DEFAULT };
		device->CreateCommittedResource(&defaultHeap, D3D12_HEAP_FLAG_NONE, &texDesc,
			D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_TextureResource));

		UINT64 uploadBufferSize = d3dUtil::GetRequiredIntermediateSize(m_TextureResource.Get(), 0, 1);
		D3D12_HEAP_PROPERTIES uploadHeap = { D3D12_HEAP_TYPE_UPLOAD };
		D3D12_RESOURCE_DESC bufferDesc = {
			D3D12_RESOURCE_DIMENSION_BUFFER,
			0, uploadBufferSize,
			1, 1, 1,
			DXGI_FORMAT_UNKNOWN,
			{1, 0},
			D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
			D3D12_RESOURCE_FLAG_NONE
		};

		device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &bufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&uploadBuffer));

		D3D12_SUBRESOURCE_DATA subresourceData = {};
		subresourceData.pData = data;
		subresourceData.RowPitch = m_Width * 4;
		subresourceData.SlicePitch = subresourceData.RowPitch * m_Height;

		d3dUtil::UpdateSubresources(tempCmdList.Get(), m_TextureResource.Get(), uploadBuffer.Get(), 0, 0, 1, &subresourceData);

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = m_TextureResource.Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		tempCmdList->ResourceBarrier(1, &barrier);

		tempCmdList->Close();
		ID3D12CommandList* cmds[] = { tempCmdList.Get() };
		queue->ExecuteCommandLists(1, cmds);

		context->FlushCommandQueue();

		stbi_image_free(data);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		auto* textureManager = RenderCommand::GetTextureManager();
		m_Handle = textureManager->CreateTextureSRV(m_TextureResource.Get(), srvDesc);

		m_IsLoaded = true;
	}

	void DirectX12Texture2D::SetData(void* data, uint32_t size) {
	}
}
