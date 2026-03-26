#pragma once

#include "Prefab.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

namespace Aurora {
	class ModelLoader {
	public:
		static std::shared_ptr<Prefab> Load(const std::filesystem::path& path);

	private:
		static std::shared_ptr<MaterialAsset> ProcessMaterial(aiMaterial* ai_mat);
		static std::shared_ptr<MeshAsset> ProcessMesh(aiMesh* ai_mesh, const aiScene* ai_scene);
		static void ProcessNode(aiNode* ai_node, const aiScene* ai_scene, ModelNode& outNode,
			const std::vector<std::shared_ptr<MeshAsset>>& loadedMeshes,
			const std::vector<std::shared_ptr<MaterialAsset>>& loadedMaterials);
	};
}
