#pragma once

#include "Aurora/Renderer/Texture.h"
#include "Platform/DirectX/Renderer/DirectX12HeapManager.h"

#include <d3d12.h>
#include <mutex>
#include <vector>

namespace Aurora {
	class DirectX12Context;

	class DirectX12TextureManager {
	public:
		DirectX12TextureManager(DirectX12Context* context, uint32_t maxTextures = 4096);
		~DirectX12TextureManager() = default;

		// register a texture in bindless heap and return an index
		TextureHandle CreateTextureSRV(ID3D12Resource* textureResource, const D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc);

		void ReleaseTextureSRV(TextureHandle handle);

		D3D12_GPU_DESCRIPTOR_HANDLE GetBindlessGPUHandle() const { return m_TextureRange.gpuBase.handle; }

	private:
		DirectX12Context* m_Context;
		DescriptorRange m_TextureRange; // huge range for all textures
		uint32_t m_MaxTextures;

		std::mutex m_Mutex;
		std::vector<uint32_t> m_FreeIndices;
		uint32_t m_NextIndex = 0;
	};
}
