#include "aupch.h"

#include "MeshInstance.h"

namespace Aurora {
	std::shared_ptr<MeshAsset> MeshInstance::GetMesh() {
		return m_Mesh;
	}

	std::shared_ptr<MaterialAsset> MeshInstance::GetBaseMaterial(uint32_t index) {
		if (index < m_Materials.size()) {
			return m_Materials[index]->GetBaseMaterial();
		}
		// TODO: return a default material to avoid nullptr exceptions
		return nullptr;
	}

	std::shared_ptr<MaterialInstance> MeshInstance::GetMaterialInstance(uint32_t index) {
		if (index < m_Materials.size()) {
			return m_Materials[index];
		}
		return nullptr;
	}

	void MeshInstance::SetMaterial(uint32_t index, const std::shared_ptr<MaterialAsset>& material) {
		if (index < m_Materials.size()) {
			m_Materials[index] = MaterialInstance::Create(material);
		}
	}

	std::shared_ptr<MeshInstance> MeshInstance::Create(const std::shared_ptr<MeshAsset>& meshAsset,
	                                                   const std::vector<std::shared_ptr<MaterialAsset>>& materials) {
		std::shared_ptr<MeshInstance> instance = std::make_shared<MeshInstance>();
		instance->m_Mesh = meshAsset;

		instance->m_Materials.reserve(materials.size());
		for (const auto& matAsset : materials) {
			instance->m_Materials.push_back(MaterialInstance::Create(matAsset));
		}
		//instance->m_Materials = materials;

		return instance;
	}
}
