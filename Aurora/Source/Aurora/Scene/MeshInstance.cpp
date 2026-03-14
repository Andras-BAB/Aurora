#include "aupch.h"

#include "MeshInstance.h"

namespace Aurora {
	std::shared_ptr<MeshAsset> MeshInstance::GetMesh() {
		return m_Mesh;
	}

	std::shared_ptr<MaterialAsset> MeshInstance::GetMaterial(uint32_t index) {
		if (index < m_Materials.size()) {
			return m_Materials[index];
		}
		// TODO: return a default material to avoid nullptr exceptions
		return nullptr;
	}

	std::shared_ptr<MeshInstance> MeshInstance::Create(const std::shared_ptr<MeshAsset>& meshAsset,
	                                                   const std::vector<std::shared_ptr<MaterialAsset>>& materials) {
		std::shared_ptr<MeshInstance> instance = std::make_shared<MeshInstance>();
		instance->m_Mesh = meshAsset;
		instance->m_Materials = materials;

		return instance;
	}
}
