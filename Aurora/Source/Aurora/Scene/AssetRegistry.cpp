#include "aupch.h"

#include "AssetRegistry.h"

namespace Aurora {
	std::shared_ptr<MeshAsset> AssetRegistry::GetMesh(Aurora::UUID uuid) {
		if (m_Meshes.contains(uuid))
			return m_Meshes[uuid];
		return nullptr;
	}

	void AssetRegistry::AddMesh(Aurora::UUID uuid, std::shared_ptr<MeshAsset> asset) {
		m_Meshes[uuid] = asset;
	}

	void AssetRegistry::AddMesh(std::shared_ptr<MeshAsset> asset) {
		m_Meshes[asset->GetUUID()] = asset;
	}

	void AssetRegistry::AddMaterial(std::shared_ptr<MaterialAsset> asset) {
		m_Materials[asset->GetUUID()] = asset;
	}

	std::shared_ptr<MaterialAsset> AssetRegistry::GetMaterial(Aurora::UUID uuid) {
		return m_Materials.contains(uuid) ? m_Materials[uuid] : nullptr;
	}

	std::unordered_map<Aurora::UUID, std::shared_ptr<MaterialAsset>>& AssetRegistry::GetMaterials() {
		return m_Materials;
	}

	void AssetRegistry::AddPrefab(const std::shared_ptr<Prefab>& prefab) {
		m_Prefabs[prefab->GetUUID()] = prefab;
	}

	void AssetRegistry::Clear() {
		m_Meshes.clear();
	}
}
