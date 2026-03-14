#pragma once
#include "Aurora/Scene/MeshAsset.h"
#include "Aurora/Scene/MaterialAsset.h"

#include "Aurora/Core/UUID.h"
#include "Aurora/Scene/Prefab.h"

namespace Aurora {
	class AssetRegistry {
	public:
		AssetRegistry() = default;

		std::shared_ptr<MeshAsset> GetMesh(Aurora::UUID uuid);

		void AddMesh(Aurora::UUID uuid, std::shared_ptr<MeshAsset> asset);
		void AddMesh(std::shared_ptr<MeshAsset> asset);

		void AddMaterial(std::shared_ptr<MaterialAsset> asset);
		std::shared_ptr<MaterialAsset> GetMaterial(Aurora::UUID uuid);
		std::unordered_map<Aurora::UUID, std::shared_ptr<MaterialAsset>>& GetMaterials();

		void AddPrefab(const std::shared_ptr<Prefab>& prefab);

		void Clear();

	private:
		std::unordered_map<Aurora::UUID, std::shared_ptr<MeshAsset>> m_Meshes;
		std::unordered_map<Aurora::UUID, std::shared_ptr<MaterialAsset>> m_Materials;
		std::unordered_map<Aurora::UUID, std::shared_ptr<Prefab>> m_Prefabs;
	};
}
