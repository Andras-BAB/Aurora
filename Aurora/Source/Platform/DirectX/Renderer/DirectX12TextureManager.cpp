#include "aupch.h"

#include "DirectX12TextureManager.h"
#include "Platform/DirectX/Renderer/DirectX12Context.h"
#include "Aurora/Core/Log.h"

namespace Aurora {

	DirectX12TextureManager::DirectX12TextureManager(DirectX12Context* context, uint32_t maxTextures)
		: m_Context(context), m_MaxTextures(maxTextures) {

		m_TextureRange = m_Context->GetHeapManager()->AllocateCBV_SRV_UAV_Persistent(m_MaxTextures);

		if (m_TextureRange.count < m_MaxTextures) {
			AU_CORE_ERROR("Could not allocate Bindless Texture Range!");
		}
	}

	TextureHandle DirectX12TextureManager::CreateTextureSRV(ID3D12Resource* textureResource, const D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc) {
		std::lock_guard<std::mutex> lock(m_Mutex);

		uint32_t index = 0xFFFFFFFF;

		if (!m_FreeIndices.empty()) {
			index = m_FreeIndices.back();
			m_FreeIndices.pop_back();
		} else if (m_NextIndex < m_MaxTextures) {
			index = m_NextIndex++;
		} else {
			AU_CORE_ERROR("Megtelt a Bindless Texture Heap!");
			return TextureHandle{ 0xFFFFFFFF };
		}

		UINT descriptorSize = m_Context->GetHeapManager()->GetCbvSrvUavIncrementSize();
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_TextureRange.cpuBase.handle;
		cpuHandle.ptr += static_cast<SIZE_T>(index) * descriptorSize;

		m_Context->GetDevice()->CreateShaderResourceView(textureResource, &srvDesc, cpuHandle);

		return TextureHandle{ index };
	}

	void DirectX12TextureManager::ReleaseTextureSRV(TextureHandle handle) {
		if (!handle.IsValid()) return;

		std::lock_guard<std::mutex> lock(m_Mutex);
		m_FreeIndices.push_back(handle.Index);

		// use a DeferTicket to not instantly overwrite released texture in runtime until GPU finishes current frame
	}
}