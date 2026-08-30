#include "aupch.h"

#include "ModelLoader.h"
#include "Platform/DirectX/Renderer/FrameResource.h"
#include "Aurora/Core/Log.h"

#include <filesystem>

#include "Aurora/Core/Application.h"
#include "Aurora/Utils/Hash.h"

namespace Aurora {

	// Assimp matrix -> D3D12/Math Mat4 conversion
	static math::Mat4 AiMatToMat4(const aiMatrix4x4& m) {
		// in Assimp matrices the translations are in a4, b4, c4 elements
		// DirectX math (XMMATRIX) uses the 4th row instead (_41, _42, _43) so we transpose it
		math::Mat4 floatMat(
			m.a1, m.a2, m.a3, m.a4,
			m.b1, m.b2, m.b3, m.b4,
			m.c1, m.c2, m.c3, m.c4,
			m.d1, m.d2, m.d3, m.d4
		);
		return math::Mat4::Transpose(floatMat);

		//DirectX::XMMATRIX mat = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&floatMat));
		//return math::Mat4(mat);
	}

	std::shared_ptr<MaterialAsset> ModelLoader::ProcessMaterial(aiMaterial* ai_mat, uint32_t matIndex, const std::string& filepath) {
		MaterialData matData;
		aiColor4D color(1.0f, 1.0f, 1.0f, 1.0f);
		ai_mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);

		matData.DiffuseAlbedo = { color.r, color.g, color.b, color.a };

		float roughness = 0.5f;
		if (ai_mat->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness) == AI_SUCCESS) {
			matData.Roughness = roughness;
		}
		
		//aiString texPath;
		//if (ai_mat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS) {
		//	std::string fullPath = texPath.C_Str();

		//	// TODO: create the texture and load
		//	// assimp returns the full path, use std::filesystem::path(fullPath).filename().string() to get the file name
		//}

		std::string matName = ai_mat->GetName().C_Str();
		if (matName.empty()) matName = "Material_" + std::to_string(matIndex);

		std::string uniqueIDString = filepath + "_Material_" + std::to_string(matIndex);
		uint64_t stableHash = Utils::HashString(uniqueIDString);
		Aurora::UUID matUUID = Aurora::UUID(stableHash);

		return MaterialAsset::Create(matName, matData, matUUID);
	}

	std::shared_ptr<MeshAsset> ModelLoader::ProcessMesh(aiMesh* ai_mesh, const aiScene* ai_scene, uint32_t meshIndex, const std::string& filepath) {
		std::string meshName = ai_mesh->mName.C_Str();
		if (meshName.empty()) meshName = "Mesh_" + std::to_string(meshIndex);

		std::string uniqueIDString = filepath + "_" + meshName + "_" + std::to_string(meshIndex);
		uint64_t stableHash = Utils::HashString(uniqueIDString);

		Aurora::UUID meshUUID = Aurora::UUID(stableHash);

		auto existingMesh = Application::Get().GetAssetRegistry().GetMesh(meshUUID);
		if (existingMesh) {
			return existingMesh;
		}

		// use masking to be able to load wider range of models
		if (!(ai_mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE)) {
			AU_CORE_WARN("Skipped a mesh with no triangles: {0}", ai_mesh->mName.C_Str());
			return nullptr;
		}

		std::vector<Vertex> vertices(ai_mesh->mNumVertices);
		std::vector<uint32_t> indices;

		for (uint32_t i = 0; i < ai_mesh->mNumVertices; i++) {
			vertices[i].Position = { ai_mesh->mVertices[i].x, ai_mesh->mVertices[i].y, ai_mesh->mVertices[i].z };

			if (ai_mesh->HasNormals())
				vertices[i].Normal = { ai_mesh->mNormals[i].x, ai_mesh->mNormals[i].y, ai_mesh->mNormals[i].z };

			if (ai_mesh->HasTangentsAndBitangents())
				vertices[i].Tangent = { ai_mesh->mTangents[i].x, ai_mesh->mTangents[i].y, ai_mesh->mTangents[i].z };

			if (ai_mesh->HasTextureCoords(0))
				vertices[i].UV = { ai_mesh->mTextureCoords[0][i].x, ai_mesh->mTextureCoords[0][i].y };
		}

		//for (uint32_t i = 0; i < ai_mesh->mNumFaces; i++) {
		//	aiFace face = ai_mesh->mFaces[i];
		//	for (uint32_t j = 0; j < face.mNumIndices; j++)
		//		indices.push_back(face.mIndices[j]);
		//}

		for (uint32_t i = 0; i < ai_mesh->mNumFaces; i++) {
			aiFace face = ai_mesh->mFaces[i];

			if (face.mNumIndices == 3) {
				indices.push_back(face.mIndices[0]);
				indices.push_back(face.mIndices[1]);
				indices.push_back(face.mIndices[2]);
			}
		}

		MeshData meshData;
		meshData.VertexData = vertices.data();
		meshData.VertexSize = static_cast<uint32_t>(vertices.size() * sizeof(Vertex));
		meshData.VertexStride = sizeof(Vertex);
		meshData.IndexData = indices.data();
		meshData.IndexSize = static_cast<uint32_t>(indices.size() * sizeof(uint32_t));

		std::shared_ptr<MeshAsset> asset = MeshAsset::Create(meshName, meshData, meshUUID);

		SubmeshGeometry sm;
		sm.IndexCount = static_cast<uint32_t>(indices.size());
		sm.StartIndexLocation = 0;
		sm.BaseVertexLocation = 0;
		asset->m_Submeshes.push_back(sm);

		SubmeshInstance inst;
		inst.Name = meshName;
		inst.SubmeshIndex = 0;
		inst.MaterialIndex = ai_mesh->mMaterialIndex;
		inst.LocalTransform = math::Mat4::Identity();
		asset->m_SubmeshInstances.push_back(inst);

		return asset;
	}

	bool ModelLoader::ProcessNode(aiNode* ai_node, const aiScene* ai_scene, ModelNode& outNode,
		const std::vector<std::shared_ptr<MeshAsset>>& loadedMeshes,
		const std::vector<std::shared_ptr<MaterialAsset>>& loadedMaterials) {

		outNode.Name = ai_node->mName.C_Str();

		std::string lowerName = outNode.Name;
		std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

		bool isHitboxOrLOD =
			lowerName.find("col") != std::string::npos ||
			lowerName.find("ucx") != std::string::npos ||
			lowerName.find("lod1") != std::string::npos ||
			lowerName.find("lod2") != std::string::npos ||
			lowerName.find("lod3") != std::string::npos;

		if (isHitboxOrLOD) {
			AU_CORE_INFO("Filtered out utility mesh entirely: {0}", outNode.Name);
			return false;
		}

		math::Mat4 localTransform = AiMatToMat4(ai_node->mTransformation);
		localTransform.Decompose(outNode.Translation, outNode.Rotation, outNode.Scale);

		// for simplicity: 1 node = 1 mesh
		if (ai_node->mNumMeshes > 0 && !isHitboxOrLOD) {
			uint32_t meshIndex = ai_node->mMeshes[0];

			auto meshAsset = loadedMeshes[meshIndex];

			if (meshAsset != nullptr) {
				outNode.Mesh = meshAsset;
				outNode.Materials = loadedMaterials;
			}

			for (uint32_t i = 1; i < ai_node->mNumMeshes; i++) {
				ModelNode extraMeshNode;
				extraMeshNode.Name = outNode.Name + "_Submesh_" + std::to_string(i);
				extraMeshNode.Mesh = loadedMeshes[ai_node->mMeshes[i]];
				extraMeshNode.Materials = loadedMaterials;
				outNode.Children.push_back(extraMeshNode);
			}
		}

		for (uint32_t i = 0; i < ai_node->mNumChildren; i++) {
			ModelNode childNode;
			if (ProcessNode(ai_node->mChildren[i], ai_scene, childNode, loadedMeshes, loadedMaterials)) {
				outNode.Children.push_back(childNode);
			}
		}
		return true;
	}

	// make a LoadStatic equivalent to be able to load to a single entity
	std::shared_ptr<Prefab> ModelLoader::Load(const std::filesystem::path& path) {
		uint64_t prefabHash = Utils::HashString(path.string() + "_Prefab");
		Aurora::UUID prefabUUID(prefabHash);

		auto existingPrefab = Application::Get().GetAssetRegistry().GetPrefab(prefabUUID);
		if (existingPrefab) {
			return existingPrefab;
		}

		Assimp::Importer importer;

		const aiScene* ai_scene = importer.ReadFile(path.string(),
			aiProcess_Triangulate |
			aiProcess_GenSmoothNormals |
			aiProcess_CalcTangentSpace |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType |
			aiProcess_ConvertToLeftHanded
			//aiProcessPreset_TargetRealtime_MaxQuality | // not used to prevent merging the materials and submeshes
			//aiProcess_ConvertToLeftHanded
		);

		if (!ai_scene || ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode) {
			AU_CORE_ERROR("Assimp Error: {0}", importer.GetErrorString());
			return nullptr;
		}

		std::shared_ptr<Prefab> prefab = std::make_shared<Prefab>();
		prefab->SetUUID(prefabUUID);

		std::vector<std::shared_ptr<MaterialAsset>> loadedMaterials;
		for (uint32_t i = 0; i < ai_scene->mNumMaterials; i++) {
			loadedMaterials.push_back(ProcessMaterial(ai_scene->mMaterials[i], i, path.string()));
		}

		std::vector<std::shared_ptr<MeshAsset>> loadedMeshes;
		for (uint32_t i = 0; i < ai_scene->mNumMeshes; i++) {
			loadedMeshes.push_back(ProcessMesh(ai_scene->mMeshes[i], ai_scene, i, path.string()));
		}

		ProcessNode(ai_scene->mRootNode, ai_scene, prefab->RootNode, loadedMeshes, loadedMaterials);

		if (prefab->RootNode.Mesh == nullptr && prefab->RootNode.Children.size() == 1) {
			AU_CORE_INFO("Collapsing empty RootNode into child: {0}", prefab->RootNode.Children[0].Name);

			ModelNode newRoot = prefab->RootNode.Children[0];
			prefab->RootNode = std::move(newRoot);
		}

		AU_CORE_INFO("Model loaded successfully: {0}", path.string());

		Application::Get().GetAssetRegistry().AddPrefab(prefab);
		Application::Get().GetAssetRegistry().RegisterAssetPath(prefab->GetUUID(), AssetType::Prefab, path.string());

		AU_CORE_INFO("Model loaded: {0}. Total Meshes: {1}, Total Materials: {2}", path.string(), ai_scene->mNumMeshes, ai_scene->mNumMaterials);
		return prefab;
	}
}
