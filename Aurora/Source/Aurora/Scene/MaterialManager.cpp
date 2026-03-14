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
		m_UUIDToGPUInfo[uuid] = { index, asset };
		
		asset->GetData().NumFramesDirty = frameCount;

		return index;
	}

	void MaterialManager::UpdateAllDirtyMaterials(UploadBuffer<MaterialConstants>* currentCB) {
		for (auto& info : m_UUIDToGPUInfo | std::views::values) {
			if (auto asset = info.Asset.lock()) {
				auto& data = asset->GetData();
				if (data.NumFramesDirty > 0) {
					MaterialConstants matConstants;
					matConstants.DiffuseAlbedo = data.DiffuseAlbedo;
					matConstants.FresnelR0 = data.FresnelR0;
					matConstants.Roughness = data.Roughness;
					XMStoreFloat4x4(&matConstants.MatTransform, XMMatrixTranspose(XMLoadFloat4x4(&data.MatTransform)));

					currentCB->CopyData(info.CBIndex, matConstants);
					data.NumFramesDirty--;
				}
			}
		}
	}

	void MaterialManager::ForceRefreshAll(uint32_t frameCount) {
		for (auto& info : m_UUIDToGPUInfo | std::views::values) {
			if (std::shared_ptr<MaterialAsset> asset = info.Asset.lock()) {
				asset->GetData().NumFramesDirty = frameCount;
			}
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
