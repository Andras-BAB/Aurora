#pragma once

#include "MaterialAsset.h"

// TODO: API specific code in API agnostic section
#include "Platform/DirectX/Renderer/FrameResource.h"

namespace Aurora {
	struct MaterialGPUInfo {
		uint32_t CBIndex;
		std::weak_ptr<MaterialAsset> Asset;
	};

	class MaterialManager {
	public:
		uint32_t GetGPUIndex(std::shared_ptr<MaterialAsset> asset, uint32_t frameCount);
		void UpdateAllDirtyMaterials(UploadBuffer<MaterialConstants>* currentCB);

		void ForceRefreshAll(uint32_t frameCount);

		uint32_t GetAllocatedCount() const;

		void ReleaseMaterial(Aurora::UUID uuid);
	private:
		uint32_t AllocateIndex();

	private:
		std::unordered_map<Aurora::UUID, MaterialGPUInfo> m_UUIDToGPUInfo;
		std::vector<uint32_t> m_FreeIndices;
		uint32_t m_NextIndex = 0;
	};
}
