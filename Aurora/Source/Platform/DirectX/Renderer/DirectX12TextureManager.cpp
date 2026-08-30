#include "aupch.h"

#include "DirectX12TextureManager.h"
#include "Platform/DirectX/Renderer/DirectX12Context.h"
#include "Aurora/Core/Log.h"

namespace Aurora {

	DirectX12TextureManager::DirectX12TextureManager(DirectX12Context* context, uint32_t maxTextures)
		: m_Context(context), m_MaxTextures(maxTextures) {

		m_TextureRange = m_Context->GetHeapManager()->AllocateCBV_SRV_UAV_Persistent(m_MaxTextures);

		if (m_TextureRange.count < m_MaxTextures) {
			AU_CORE_ERROR("Cannot allocate Bindless Texture Range!");
		}
	}

	TextureHandle DirectX12TextureManager::CreateTextureSRV(ID3D12Resource* textureResource, const D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc) {
		std::lock_guard<std::mutex> lock(m_Mutex);

		uint32_t index = AllocateIndexInternal();

		if (index == 0xFFFFFFFF) return TextureHandle{ 0xFFFFFFFF };

		UINT descriptorSize = m_Context->GetHeapManager()->GetCbvSrvUavIncrementSize();
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_TextureRange.cpuBase.handle;
		cpuHandle.ptr += static_cast<SIZE_T>(index) * descriptorSize;

		m_Context->GetDevice()->CreateShaderResourceView(textureResource, &srvDesc, cpuHandle);

		return TextureHandle{ index };
	}

	void DirectX12TextureManager::ReleaseTextureSRV(TextureHandle handle) {
		if (!handle.IsValid()) return;

		UINT64 safeFence = m_Context->GetNextFenceValue();

		std::scoped_lock<std::mutex> lock(m_Mutex);
		// TODO: use a DeferTicket to not instantly overwrite released texture in runtime until GPU finishes current frame
		m_DeferredReleases.push_back({ handle.Index, safeFence });
	}

	TextureHandle DirectX12TextureManager::AllocateDescriptor() {
		std::scoped_lock<std::mutex> lock(m_Mutex);
		return TextureHandle { AllocateIndexInternal() };
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DirectX12TextureManager::GetCPUHandle(TextureHandle handle) const {
		UINT size = m_Context->GetHeapManager()->GetCbvSrvUavIncrementSize();
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_TextureRange.cpuBase.handle;
		cpuHandle.ptr += static_cast<SIZE_T>(handle.Index) * size;
		return cpuHandle;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE DirectX12TextureManager::GetGPUHandle(TextureHandle handle) const {
		UINT size = m_Context->GetHeapManager()->GetCbvSrvUavIncrementSize();
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = m_TextureRange.gpuBase.handle;
		gpuHandle.ptr += static_cast<UINT64>(handle.Index) * size;
		return gpuHandle;
	}

	ID3D12DescriptorHeap* DirectX12TextureManager::GetBindlessHeap() const {
		return m_Context->GetHeapManager()->GetBindlessSrvHeap();
	}

	uint32_t DirectX12TextureManager::AllocateIndexInternal() {
		uint32_t index = 0xFFFFFFFF;
		if (!m_FreeIndices.empty()) {
			index = m_FreeIndices.back();
			m_FreeIndices.pop_back();
		} else if (m_NextIndex < m_MaxTextures) {
			index = m_NextIndex++;
		} else {
			AU_CORE_ERROR("Bindless Heap is full! Allocation failed!");
		}
		return index;
	}

	void DirectX12TextureManager::ProcessDeferredReleases() {
		std::scoped_lock<std::mutex> lock(m_Mutex);

		UINT64 completedFence = m_Context->GetCompletedFenceValue();

		auto it = m_DeferredReleases.begin();
		while (it != m_DeferredReleases.end()) {
			if (it->SafeToReleaseFence <= completedFence) {
				m_FreeIndices.push_back(it->Index);
				it = m_DeferredReleases.erase(it);
			} else {
				++it;
			}
		}
	}
}
