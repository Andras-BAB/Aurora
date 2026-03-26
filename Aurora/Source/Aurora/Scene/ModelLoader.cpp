#include "aupch.h"

#include "ModelLoader.h"
#include "Platform/DirectX/Renderer/FrameResource.h"
#include "Aurora/Core/Log.h"

#include <filesystem>

#include "Aurora/Core/Application.h"

namespace Aurora {

	// Assimp matrix -> D3D12/Math Mat4 conversion
	static math::Mat4 AiMatToMat4(const aiMatrix4x4& m) {
		// in Assimp matrices the translations are in a4, b4, c4 elements
		// DirectX math (XMMATRIX) uses the 4th row instead (_41, _42, _43) so we transpose it
		DirectX::XMFLOAT4X4 floatMat(
			m.a1, m.a2, m.a3, m.a4,
			m.b1, m.b2, m.b3, m.b4,
			m.c1, m.c2, m.c3, m.c4,
			m.d1, m.d2, m.d3, m.d4
		);
		DirectX::XMMATRIX mat = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&floatMat));
		return math::Mat4(mat);
	}

	std::shared_ptr<MaterialAsset> ModelLoader::ProcessMaterial(aiMaterial* ai_mat) {
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
		if (matName.empty()) matName = "Material_" + std::to_string(rand() % 10000);

		return MaterialAsset::Create(matName, matData);
	}

	std::shared_ptr<MeshAsset> ModelLoader::ProcessMesh(aiMesh* ai_mesh, const aiScene* ai_scene) {
		//if (ai_mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE) {
		//	AU_CORE_WARN("Skipped a non-triangle mesh (lines/points): {0}", ai_mesh->mName.C_Str());
		//	return nullptr;
		//}

		// use masking to be able to load wider range of models
		if (!(ai_mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE)) {
			AU_CORE_WARN("Skipped a mesh with NO triangles: {0}", ai_mesh->mName.C_Str());
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

		std::string meshName = ai_mesh->mName.C_Str();
		if (meshName.empty()) meshName = "Mesh_" + std::to_string(rand() % 10000);

		std::shared_ptr<MeshAsset> asset = MeshAsset::Create(meshName, meshData);

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

	void ModelLoader::ProcessNode(aiNode* ai_node, const aiScene* ai_scene, ModelNode& outNode,
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

		math::Mat4 localTransform = AiMatToMat4(ai_node->mTransformation);
		// here we can use decomposition (Translation/Rotation/Scale), 

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
		} else if (isHitboxOrLOD) {
			AU_CORE_INFO("Filtered out utility mesh: {0}", outNode.Name);
		}

		for (uint32_t i = 0; i < ai_node->mNumChildren; i++) {
			ModelNode childNode;
			ProcessNode(ai_node->mChildren[i], ai_scene, childNode, loadedMeshes, loadedMaterials);
			outNode.Children.push_back(childNode);
		}
	}

	// make a LoadStatic equivalent to be able to load to a single entity
	std::shared_ptr<Prefab> ModelLoader::Load(const std::filesystem::path& path) {
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

		std::vector<std::shared_ptr<MaterialAsset>> loadedMaterials;
		for (uint32_t i = 0; i < ai_scene->mNumMaterials; i++) {
			loadedMaterials.push_back(ProcessMaterial(ai_scene->mMaterials[i]));
		}

		std::vector<std::shared_ptr<MeshAsset>> loadedMeshes;
		for (uint32_t i = 0; i < ai_scene->mNumMeshes; i++) {
			loadedMeshes.push_back(ProcessMesh(ai_scene->mMeshes[i], ai_scene));
		}

		ProcessNode(ai_scene->mRootNode, ai_scene, prefab->RootNode, loadedMeshes, loadedMaterials);

		AU_CORE_INFO("Model loaded successfully: {0}", path.string());

		Application::Get().GetAssetRegistry().AddPrefab(prefab);
		AU_CORE_INFO("Model loaded: {0}. Total Meshes: {1}, Total Materials: {2}", path.string(), ai_scene->mNumMeshes, ai_scene->mNumMaterials);
		return prefab;
	}
}
