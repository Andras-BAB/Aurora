#include "aupch.h"

#include "MaterialManager.h"

#include <ranges>

namespace Aurora {

	uint32_t MaterialManager::GetGPUIndex(std::shared_ptr<MaterialAsset> asset, uint32_t frameCount) {
		if (!asset) return 0;

		Aurora::UUID uuid = asset->GetUUID();

		if (m_UUIDToGPUInfo.contains(uuid)) {
			return m_UUIDToGPUInfo[uuid].CBIndex;
		}

		uint32_t index = AllocateIndex();
		m_UUIDToGPUInfo[uuid] = { index, asset, 0, frameCount };
		
		return index;
	}

	void MaterialManager::UpdateAllDirtyMaterials(UploadBuffer<MaterialConstants>* currentCB) {
		for (auto& info : m_UUIDToGPUInfo | std::views::values) {
			if (auto asset = info.Asset.lock()) {
				if (asset->GetVersion() != info.LastSeenVersion) {
					info.LastSeenVersion = asset->GetVersion();
					// TODO: make it dynamic
					info.NumFramesDirty = 3;
				}

				if (info.NumFramesDirty > 0) {
					auto& data = asset->GetData();

					MaterialConstants matConstants;
					matConstants.DiffuseAlbedo = data.DiffuseAlbedo;
					matConstants.FresnelR0 = data.FresnelR0;
					matConstants.Roughness = data.Roughness;
					matConstants.DiffuseMapIndex = data.DiffuseMapIndex;
					XMStoreFloat4x4(&matConstants.MatTransform, DirectX::XMMatrixTranspose(data.MatTransform));

					currentCB->CopyData(info.CBIndex, matConstants);
					info.NumFramesDirty--;
				}
			}
		}
	}

	void MaterialManager::ForceRefreshAll(uint32_t frameCount) {
		for (auto& info : m_UUIDToGPUInfo | std::views::values) {
			info.NumFramesDirty = frameCount;
		}
	}

	uint32_t MaterialManager::GetAllocatedCount() const {
		return m_NextIndex;
	}

	void MaterialManager::ReleaseMaterial(Aurora::UUID uuid) {
		if (m_UUIDToGPUInfo.contains(uuid)) {
			m_FreeIndices.push_back(m_UUIDToGPUInfo[uuid].CBIndex);
			m_UUIDToGPUInfo.erase(uuid);
		}
	}

	uint32_t MaterialManager::AllocateIndex() {
		if (!m_FreeIndices.empty()) {
			uint32_t idx = m_FreeIndices.back();
			m_FreeIndices.pop_back();
			return idx;
		}
		return m_NextIndex++;
	}

}
