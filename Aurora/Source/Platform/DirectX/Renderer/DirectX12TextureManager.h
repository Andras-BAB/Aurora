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
		// maxTextures: Mekkora legyen a globális bindless textúra tömbünk (pl. 4096)
		DirectX12TextureManager(DirectX12Context* context, uint32_t maxTextures = 4096);
		~DirectX12TextureManager() = default;

		// Beregisztrál egy textúrát a bindless heap-be, és visszaadja az indexet
		TextureHandle CreateTextureSRV(ID3D12Resource* textureResource, const D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc);

		// Felszabadítja az indexet (amikor a textúra destruktora lefut)
		void ReleaseTextureSRV(TextureHandle handle);

		// A render loophoz: lekérjük a teljes blokk GPU címét
		D3D12_GPU_DESCRIPTOR_HANDLE GetBindlessGPUHandle() const { return m_TextureRange.gpuBase.handle; }

	private:
		DirectX12Context* m_Context;
		DescriptorRange m_TextureRange; // A hatalmas, egybefüggő descriptor tartomány
		uint32_t m_MaxTextures;

		std::mutex m_Mutex;
		std::vector<uint32_t> m_FreeIndices;
		uint32_t m_NextIndex = 0;
	};
}
