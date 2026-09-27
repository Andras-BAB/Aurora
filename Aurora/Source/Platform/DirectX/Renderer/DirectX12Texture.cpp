#include "aupch.h"

#include "DirectX12Texture.h"

#include <stb_image.h>

#include "DirectX12RenderCommand.h"
#include "Aurora/Core/Application.h"

namespace Aurora {
	DirectX12Texture2D::DirectX12Texture2D(const TextureSpecification& specification) {
		m_UUID = UUID();

		auto* context = DirectX12RenderCommand::GetContext();
		auto* device = context->GetDevice();
		auto* allocator = context->GetAllocator();

		m_Width = specification.Width;
		m_Height = specification.Height;

		D3D12_RESOURCE_DESC texDesc = {};
		texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		texDesc.Alignment = 0;
		texDesc.Width = m_Width;
		texDesc.Height = m_Height;
		texDesc.DepthOrArraySize = 1;
		texDesc.MipLevels = 1;
		texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // TODO: specification.Format
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		D3D12MA::ALLOCATION_DESC defaultAllocDesc = {};
		defaultAllocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

		MS::ComPtr<ID3D12Resource> physicalResource;
		ThrowOnFail(allocator->CreateResource(
			&defaultAllocDesc,
			&texDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			m_TextureAllocation.GetAddressOf(),
			IID_PPV_ARGS(&physicalResource)
		));

		//D3D12_HEAP_PROPERTIES defaultHeap = { D3D12_HEAP_TYPE_DEFAULT };
		//device->CreateCommittedResource(&defaultHeap, D3D12_HEAP_FLAG_NONE, &texDesc,
		//	D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_TextureResource));

		//UINT64 uploadBufferSize = d3dUtil::GetRequiredIntermediateSize(m_TextureResource.Get(), 0, 1);
		//D3D12_HEAP_PROPERTIES uploadHeap = { D3D12_HEAP_TYPE_UPLOAD };
		//D3D12_RESOURCE_DESC bufferDesc = {
		//	D3D12_RESOURCE_DIMENSION_BUFFER,
		//	0, uploadBufferSize,
		//	1, 1, 1,
		//	DXGI_FORMAT_UNKNOWN,
		//	{1, 0},
		//	D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
		//	D3D12_RESOURCE_FLAG_NONE
		//};

		//device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &bufferDesc,
		//	D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&uploadBuffer));

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		auto* textureManager = RenderCommand::GetTextureManager();
		m_Handle = textureManager->CreateTextureSRV(physicalResource.Get(), srvDesc);

		m_IsLoaded = false;
	}

	DirectX12Texture2D::DirectX12Texture2D(const std::string& path, Aurora::UUID uuid) : m_Path(path) {
		m_UUID = uuid;

		auto* context = DirectX12RenderCommand::GetContext();
		auto* allocator = context->GetAllocator();

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

		D3D12MA::ALLOCATION_DESC defaultAllocDesc = {};
		defaultAllocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

		MS::ComPtr<ID3D12Resource> physicalResource = nullptr;
		ThrowOnFail(allocator->CreateResource(&defaultAllocDesc, &texDesc,
			D3D12_RESOURCE_STATE_COPY_DEST, nullptr, m_TextureAllocation.GetAddressOf(), IID_PPV_ARGS(&physicalResource)));

		auto* uploader = RenderCommand::GetRendererAPIAs<DirectX12RendererAPI>()->GetUploadContext();
		uploader->UploadTexture(physicalResource.Get(), data, m_Width * 4, static_cast<uint64_t>(m_Width) * 4 * m_Height,
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		stbi_image_free(data);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		auto* textureManager = RenderCommand::GetTextureManager();
		m_Handle = textureManager->CreateTextureSRV(physicalResource.Get(), srvDesc);

		m_IsLoaded = true;
	}

	DirectX12Texture2D::~DirectX12Texture2D() {
		if (m_Handle.IsValid()) {
			RenderCommand::GetTextureManager()->ReleaseTextureSRV(m_Handle);
		}
	}

	void DirectX12Texture2D::SetData(void* data, uint32_t size) {
		ID3D12Resource* physicalResource = m_TextureAllocation->GetResource();

		D3D12_RESOURCE_STATES stateBefore = m_IsLoaded ? D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE : D3D12_RESOURCE_STATE_COPY_DEST;

		auto* uploader = RenderCommand::GetRendererAPIAs<DirectX12RendererAPI>()->GetUploadContext();
		uploader->UploadTexture(physicalResource, data, m_Width * 4, static_cast<uint64_t>(m_Width) * 4 * m_Height,
			stateBefore, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		if (!m_IsLoaded) {
			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = 1;

			auto* textureManager = RenderCommand::GetTextureManager();
			m_Handle = textureManager->CreateTextureSRV(physicalResource, srvDesc);

			m_IsLoaded = true;
		}
	}
}
