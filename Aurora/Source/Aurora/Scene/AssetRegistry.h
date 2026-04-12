#pragma once
#include "Aurora/Scene/MeshAsset.h"
#include "Aurora/Scene/MaterialAsset.h"
#include "Aurora/Renderer/Texture.h"

#include "Aurora/Core/UUID.h"
#include "Aurora/Scene/Prefab.h"

namespace Aurora {

	enum class AssetType { None = 0, Mesh, Texture, Material, Prefab };

	struct AssetMetadata {
		UUID Handle;
		AssetType Type;
		std::string FilePath;
	};

	class AssetRegistry {
	public:
		AssetRegistry() = default;

		bool LoadDatabase(const std::string& databasePath = "data/assets.yaml");
		bool SaveDatabase(const std::string& databasePath = "data/assets.yaml");

		void RegisterAssetPath(UUID uuid, AssetType type, const std::string& path);
		std::string GetAssetPath(UUID uuid) const;

		std::shared_ptr<ITexture2D> GetTexture(Aurora::UUID uuid);
		void AddTexture(Aurora::UUID uuid, std::shared_ptr<ITexture2D> texture);

		std::shared_ptr<Prefab> GetPrefab(Aurora::UUID uuid);
		void AddPrefab(Aurora::UUID uuid, std::shared_ptr<Prefab> prefab);


		std::shared_ptr<MeshAsset> GetMesh(Aurora::UUID uuid);

		void AddMesh(Aurora::UUID uuid, std::shared_ptr<MeshAsset> asset);
		void AddMesh(std::shared_ptr<MeshAsset> asset);

		void AddMaterial(std::shared_ptr<MaterialAsset> asset);
		std::shared_ptr<MaterialAsset> GetMaterial(Aurora::UUID uuid);
		std::unordered_map<Aurora::UUID, std::shared_ptr<MaterialAsset>>& GetMaterials();

		void AddPrefab(const std::shared_ptr<Prefab>& prefab);

		void Clear();

	private:
		std::unordered_map<Aurora::UUID, AssetMetadata> m_AssetRegistryMap;

		std::unordered_map<Aurora::UUID, std::shared_ptr<MeshAsset>> m_Meshes;
		std::unordered_map<Aurora::UUID, std::shared_ptr<MaterialAsset>> m_Materials;
		std::unordered_map<Aurora::UUID, std::shared_ptr<Prefab>> m_Prefabs;
		std::unordered_map<Aurora::UUID, std::shared_ptr<ITexture2D>> m_Textures;
	};
}
