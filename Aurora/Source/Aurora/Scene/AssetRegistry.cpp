#include "aupch.h"

#include "AssetRegistry.h"

#include <yaml-cpp/yaml.h>

namespace Aurora {
	bool AssetRegistry::LoadDatabase(const std::string& databasePath) {
		YAML::Node data;
		try {
			data = YAML::LoadFile(databasePath);
		} catch (YAML::ParserException& e) {
			AU_CORE_WARN("Cannot parse assets.yaml: {0}, from path:", e.msg, databasePath);
			return false;
		} catch (YAML::BadFile& e) {
			AU_CORE_WARN("Cannot find assets.yaml at location: {0}", databasePath);
			return false;
		}

		auto assetsNode = data["Assets"];
		if (assetsNode) {
			for (auto asset : assetsNode) {
				UUID uuid = asset["UUID"].as<uint64_t>();
				AssetType type = (AssetType)asset["Type"].as<int>();
				std::string path = asset["Path"].as<std::string>();

				m_AssetRegistryMap[uuid] = { uuid, type, path };
			}
		}
		AU_CORE_INFO("Asset Database loaded: {0} records.", m_AssetRegistryMap.size());
		return true;
	}

	bool AssetRegistry::SaveDatabase(const std::string& databasePath) {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Assets" << YAML::Value << YAML::BeginSeq;

		std::vector<std::pair<Aurora::UUID, AssetMetadata>> ordered;
		ordered.reserve(m_AssetRegistryMap.size());
		for (const auto& [uuid, metadata] : m_AssetRegistryMap) {
			ordered.emplace_back(uuid, metadata);
		}
		std::ranges::sort(ordered, [](const auto& a, const auto& b) {
			return a.first < b.first;
		});

		//for (const auto& [uuid, metadata] : m_AssetRegistryMap) {
		//	out << YAML::BeginMap;
		//	out << YAML::Key << "UUID" << YAML::Value << (uint64_t)metadata.Handle;
		//	out << YAML::Key << "Type" << YAML::Value << (int)metadata.Type;
		//	out << YAML::Key << "Path" << YAML::Value << metadata.FilePath;
		//	out << YAML::EndMap;
		//}

		for (const auto& [uuid, metadata] : ordered) {
			out << YAML::BeginMap;
			out << YAML::Key << "UUID" << YAML::Value << (uint64_t)metadata.Handle;
			out << YAML::Key << "Type" << YAML::Value << (int)metadata.Type;
			out << YAML::Key << "Path" << YAML::Value << metadata.FilePath;
			out << YAML::EndMap;
		}

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::filesystem::path filepath = databasePath;
		if (filepath.has_parent_path()) {
			std::filesystem::create_directories(filepath.parent_path());
		}

		std::ofstream fout(databasePath);
		fout << out.c_str();
		return true;
	}

	void AssetRegistry::RegisterAssetPath(UUID uuid, AssetType type, const std::string& path) {
		m_AssetRegistryMap[uuid] = { uuid, type, path };
	}

	std::string AssetRegistry::GetAssetPath(UUID uuid) const {
		if (m_AssetRegistryMap.contains(uuid)) {
			return m_AssetRegistryMap.at(uuid).FilePath;
		}
		return "";
	}

	std::shared_ptr<ITexture2D> AssetRegistry::GetTexture(Aurora::UUID uuid) {
		if (m_Textures.contains(uuid)) return m_Textures[uuid];
		return nullptr;
	}

	void AssetRegistry::AddTexture(Aurora::UUID uuid, std::shared_ptr<ITexture2D> texture) {
		m_Textures[uuid] = texture;
	}

	std::shared_ptr<Prefab> AssetRegistry::GetPrefab(Aurora::UUID uuid) {
		if (m_Prefabs.contains(uuid)) return m_Prefabs[uuid];
		return nullptr;
	}

	void AssetRegistry::AddPrefab(Aurora::UUID uuid, std::shared_ptr<Prefab> prefab) {
		m_Prefabs[uuid] = prefab;
	}

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
