#include "aupch.h"

#include "MeshAsset.h"

#include "Aurora/Core/Application.h"
#include "Platform/DirectX/Renderer/FrameResource.h"
#include "Aurora/Renderer/RenderCommand.h"
#include "Platform/DirectX/Utils/GeometryGenerator.h"

#include <tiny_gltf.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Aurora {
	static void TestAssimpIntegration() {
		Assimp::Importer importer;

		const char* dummyPath = "nem_letezo_modell.obj";

		// A post-processing flagek nagyon fontosak! 
		// aiProcess_Triangulate: Garantálja, hogy minden polygon háromszög lesz (DX12 miatt kötelező)
		// aiProcess_ConvertToLeftHanded: DirectX (bal kezes) koordinátarendszerre alakít!
		// Az aiProcess_Triangulate a leggyakoribb flag (háromszögesíti a sokszögeket)
		const aiScene* scene = importer.ReadFile(dummyPath, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene) {
			AU_CORE_TRACE("[SIKER] Az Assimp fut, es helyesen hibat dobott a hianyzo fajlra:");
			AU_CORE_TRACE("Hiba: {0}", importer.GetErrorString());
		} else {
			AU_CORE_TRACE("[HIBA] Valami fura tortent, betoltott egy nem is letezo fajlt?!");
		}
	}

	static std::vector<std::shared_ptr<MaterialAsset>> LoadMaterialsFromGLTF(const tinygltf::Model& model) {
		std::vector<std::shared_ptr<MaterialAsset>> materials;

		for (size_t i = 0; i < model.materials.size(); ++i) {
			const auto& mat = model.materials[i];

			MaterialData matData;

			auto& pbr = mat.pbrMetallicRoughness;

			matData.DiffuseAlbedo = math::Vec4(
				static_cast<float>(pbr.baseColorFactor[0]),
				static_cast<float>(pbr.baseColorFactor[1]),
				static_cast<float>(pbr.baseColorFactor[2]),
				static_cast<float>(pbr.baseColorFactor[3])
			);

			matData.Roughness = static_cast<float>(pbr.roughnessFactor);

			float metallic = static_cast<float>(pbr.metallicFactor);
			DirectX::XMFLOAT3 F0_Dielectric = { 0.04f, 0.04f, 0.04f };

			//matData.FresnelR0.x = MathHelper::Lerp(F0_Dielectric.x, matData.DiffuseAlbedo.x(), metallic);
			//matData.FresnelR0.y = MathHelper::Lerp(F0_Dielectric.y, matData.DiffuseAlbedo.y(), metallic);
			//matData.FresnelR0.z = MathHelper::Lerp(F0_Dielectric.z, matData.DiffuseAlbedo.z(), metallic);
			float x = MathHelper::Lerp(F0_Dielectric.x, matData.DiffuseAlbedo.x(), metallic);
			float y = MathHelper::Lerp(F0_Dielectric.y, matData.DiffuseAlbedo.y(), metallic);
			float z = MathHelper::Lerp(F0_Dielectric.z, matData.DiffuseAlbedo.z(), metallic);
			matData.FresnelR0 = math::Vec3(x, y, z);

			std::string matName = mat.name.empty() ? "Material_" + std::to_string(i) : mat.name;
			materials.push_back(MaterialAsset::Create(matName, matData));
		}
		return materials;
	}
	/*
	static void ProcessGLTFNode(const tinygltf::Model& model, int nodeIndex, std::shared_ptr<SceneNode> parentNode, std::shared_ptr<MeshAsset> meshAsset, const std::vector<std::shared_ptr<MaterialAsset>>& materials) {
		const auto& node = model.nodes[nodeIndex];

		auto sceneNode = SceneNode::Create(node.name);
		parentNode->AddChild(sceneNode);

		DirectX::XMFLOAT4X4 localTransform = MathHelper::Identity4x4();

		if (node.matrix.size() == 16) {
			// Ha közvetlen mátrix van megadva
			// Figyelem: GLTF column-major, DirectX row-major lehet konverzió kellhet
			// De a tinygltf double tömböt ad, float-ra kell konvertálni
			for (int i = 0; i < 16; ++i) localTransform.m[i / 4][i % 4] = static_cast<float>(node.matrix[i]);
		} else {
			DirectX::XMVECTOR translation = DirectX::XMVectorSet(0, 0, 0, 1);
			DirectX::XMVECTOR rotation = DirectX::XMVectorSet(0, 0, 0, 1); // Quaternion
			DirectX::XMVECTOR scale = DirectX::XMVectorSet(1, 1, 1, 0);

			if (node.translation.size() == 3) {
				translation = DirectX::XMVectorSet((float)node.translation[0], (float)node.translation[1], (float)node.translation[2], 1.0f);
			}
			if (node.rotation.size() == 4) {
				rotation = DirectX::XMVectorSet((float)node.rotation[0], (float)node.rotation[1], (float)node.rotation[2], (float)node.rotation[3]);
			}
			if (node.scale.size() == 3) {
				scale = DirectX::XMVectorSet((float)node.scale[0], (float)node.scale[1], (float)node.scale[2], 0.0f);
			}

			DirectX::XMMATRIX T = DirectX::XMMatrixTranslationFromVector(translation);
			DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(rotation);
			DirectX::XMMATRIX S = DirectX::XMMatrixScalingFromVector(scale);

			// Fontos: LH vs RH koordinátarendszer korrekciót itt is el kell végezni, 
			// ha a vertexeknél Z-t negáltunk, itt a Z pozíciót és a forgatást is tükrözni kell!

			DirectX::XMStoreFloat4x4(&localTransform, S * R * T);
		}

		sceneNode->SetLocalTransform(localTransform);

		// 3. Mesh hozzárendelés
		if (node.mesh >= 0) {
			// Itt jön a trükk: A te MeshAsseted jelenleg EGY nagy buffer.
			// Azt kellene tárolni, hogy melyik Submesht használja ez a node.
			// Egyszerűsítés: Most csak beállítjuk a MeshAssetet a Node-nak, 
			// de ideális esetben a SceneNode-nak tudnia kéne, melyik Submesh-t rajzolja.

			//sceneNode->SetMesh(meshAsset);
			// + Tárolni kéne: int submeshIndex = node.mesh;
		}

		// 4. Gyerekek rekurzív feldolgozása
		for (int childIndex : node.children) {
			ProcessGLTFNode(model, childIndex, sceneNode, meshAsset, materials);
		}
	}

	std::shared_ptr<MeshAsset> MeshAsset::LoadFromFile(const std::filesystem::path& path) {
		// TODO: implement advanced model loading from file

		if (path.extension() != ".glb" || path.extension() != ".gltf") {
			AU_CORE_LOG_ERROR("Currently the only supported format is glb!");
			return nullptr;
		}

		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string error;
		std::string warn;
		bool ret = false;

		if (path.extension() == ".glb") {
			ret = loader.LoadBinaryFromFile(&model, &error, &warn, path.string());
		} else {
			ret = loader.LoadASCIIFromFile(&model, &error, &warn, path.string());
		}

		if (!warn.empty()) {
			AU_CORE_LOG_WARN("Warning when loading model {0}: {1}", path.c_str(), warn.c_str());
		}
		if (!error.empty()) {
			AU_CORE_LOG_ERROR("Error when loading model {0}: {1}", path.c_str(), error.c_str());
		}
		if (!ret) {
			AU_CORE_LOG_ERROR("Failed to parse model {0}", path.c_str());
		}

		std::vector<std::shared_ptr<MaterialAsset>> loadedMaterials = LoadMaterialsFromGLTF(model);

		MaterialData defaultMaterialData{};
		auto defaultMaterial = MaterialAsset::Create("Default_GLTF_Mat", defaultMaterialData);

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<d3dUtil::SubmeshGeometry> submeshes;
		std::vector<SubmeshInstance> submeshInstances;

		//MeshData meshData{};

		for (const auto& mesh : model.meshes) {
			for (const auto& primitive : mesh.primitives) {
				const float* positions = nullptr;
				const float* normals = nullptr;
				const float* tangents = nullptr;
				const float* texCoords = nullptr;
				size_t vertexCount = 0;

				if (primitive.attributes.contains("POSITION")) {
					const auto& accessor = model.accessors[primitive.attributes.at("POSITION")];
					const auto& bufferView = model.bufferViews[accessor.bufferView];
					positions = reinterpret_cast<const float*>(&model.buffers[bufferView.buffer].data[accessor.byteOffset + bufferView.byteOffset]);
					vertexCount = accessor.count;
				}

				if (primitive.attributes.contains("NORMAL")) {
					const auto& accessor = model.accessors[primitive.attributes.at("NORMAL")];
					const auto& bufferView = model.bufferViews[accessor.bufferView];
					normals = reinterpret_cast<const float*>(&model.buffers[bufferView.buffer].data[accessor.byteOffset + bufferView.byteOffset]);
				}

				if (primitive.attributes.contains("TANGENT")) {
					const auto& accessor = model.accessors[primitive.attributes.at("TANGENT")];
					const auto& bufferView = model.bufferViews[accessor.bufferView];
					tangents = reinterpret_cast<const float*>(&model.buffers[bufferView.buffer].data[accessor.byteOffset + bufferView.byteOffset]);
				}

				if (primitive.attributes.contains("TEXCOORD_0")) {
					const auto& accessor = model.accessors[primitive.attributes.at("TEXCOORD_0")];
					const auto& bufferView = model.bufferViews[accessor.bufferView];
					texCoords = reinterpret_cast<const float*>(&model.buffers[bufferView.buffer].data[accessor.byteOffset + bufferView.byteOffset]);
				}

				uint32_t baseVertexLocation = static_cast<uint32_t>(vertices.size());

				for (size_t i = 0; i < vertexCount; ++i) {
					// z flipping for RH to LH conversion
					Vertex v;
					if (positions) {
						v.Position = DirectX::XMFLOAT3(positions[i * 3 + 0], positions[i * 3 + 1], positions[i * 3 + 2] * -1.0f);
					}

					if (normals) {
						v.Normal = DirectX::XMFLOAT3(normals[i * 3 + 0], normals[i * 3 + 1], normals[i * 3 + 2] * -1.0f);
					} else {
						v.Normal = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
					}

					if (tangents) {
						v.Tangent = DirectX::XMFLOAT3(tangents[i * 4 + 0], tangents[i * 4 + 1], tangents[i * 4 + 2] * -1.0f);
						// A W komponens (tangents[i*4+3]) a binormal irányát jelzi, azt itt most nem tároljuk a Vertex structban, de hasznos lehet.
					} else {
						v.Tangent = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
					}

					if (texCoords) {
						v.UV = DirectX::XMFLOAT2(texCoords[i * 2 + 0], texCoords[i * 2 + 1]);
					} else {
						v.UV = DirectX::XMFLOAT2(0.0f, 0.0f);
					}

					vertices.push_back(v);
				}

				uint32_t startIndexLocation = static_cast<uint32_t>(indices.size());
				uint32_t indexCount = 0;

				if (primitive.indices >= 0) {
					const auto& accessor = model.accessors[primitive.indices];
					const auto& bufferView = model.bufferViews[accessor.bufferView];
					const auto& buffer = model.buffers[bufferView.buffer];

					indexCount = (uint32_t)accessor.count;
					const void* dataPtr = &buffer.data[accessor.byteOffset + bufferView.byteOffset];

					// GLTF támogat 8, 16 és 32 bites indexeket is
					switch (accessor.componentType) {
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
					{
						const uint32_t* buf = static_cast<const uint32_t*>(dataPtr);
						for (size_t i = 0; i < accessor.count; ++i) indices.push_back(buf[i]); // BaseVertexLocation-t a Draw hívásnál kezeljük vagy itt adjuk hozzá? 
						// DX12-ben a DrawIndexedInstanced paramétere a BaseVertexLocation. 
						// Így az index pufferbe az EREDETI (0-tól induló) indexek kellenek, 
						// a globális offsetet a SubmeshGeometry tárolja.
						break;
					}
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
					{
						const uint16_t* buf = static_cast<const uint16_t*>(dataPtr);
						for (size_t i = 0; i < accessor.count; ++i) indices.push_back(buf[i]);
						break;
					}
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
					{
						const uint8_t* buf = static_cast<const uint8_t*>(dataPtr);
						for (size_t i = 0; i < accessor.count; ++i) indices.push_back(buf[i]);
						break;
					}
					default:
						AU_CORE_LOG_ERROR("Index component type not supported!");
						return nullptr;
					}
				}

				// --- Submesh Létrehozása ---
				d3dUtil::SubmeshGeometry sm;
				sm.IndexCount = indexCount;
				sm.StartIndexLocation = startIndexLocation;
				sm.BaseVertexLocation = baseVertexLocation;

				// Bounding Box számítás (opcionális, de ajánlott)
				// sm.Bounds = ... 

				submeshes.push_back(sm);

				// --- Instance létrehozása (Material hozzárendelés) ---
				SubmeshInstance instance;
				instance.Name = mesh.name;
				instance.SubmeshIndex = (uint32_t)submeshes.size() - 1;
				instance.LocalTransform = MathHelper::Identity4x4(); // Itt kéne alkalmazni a node transformot, ha lenne

				if (primitive.material >= 0 && primitive.material < loadedMaterials.size()) {
					instance.Material = loadedMaterials[primitive.material];
				} else {
					instance.Material = defaultMaterial;
				}

				submeshInstances.push_back(instance);
			}
		}

		MeshData meshData;
		meshData.VertexData = vertices.data();
		meshData.VertexStride = sizeof(Vertex);
		meshData.VertexSize = static_cast<uint32_t>(vertices.size() * sizeof(Vertex));
		meshData.IndexData = indices.data();
		meshData.IndexSize = static_cast<uint32_t>(indices.size() * sizeof(uint32_t));

		std::shared_ptr<MeshAsset> asset = Create(path.filename().string(), meshData);
		asset->m_Submeshes = submeshes;
		asset->m_SubmeshInstances = submeshInstances;

		std::shared_ptr<SceneNode> rootSceneNode = SceneNode::Create("GLTF_Root");
		const tinygltf::Scene& scene = model.scenes[model.defaultScene > -1 ? model.defaultScene : 0];
		for (int nodeIndex : scene.nodes) {
			ProcessGLTFNode(model, nodeIndex, rootSceneNode, asset, loadedMaterials);
		}

		rootSceneNode->UpdateWorldTransform(MathHelper::Identity4x4());

		return asset;
	}
	*/

	MeshAsset::MeshAsset(const std::string& name, const MeshData& meshData) {
		m_Allocation = RenderCommand::AllocateMesh(meshData);
	}

	std::shared_ptr<MaterialAsset> MeshAsset::GetMaterialForSubmesh(uint32_t submeshIndex) const {
		for (const auto& inst : m_SubmeshInstances) {
			if (inst.SubmeshIndex == submeshIndex) {
				//return inst.Material;
			}
		}
		return nullptr; // Vagy egy default "pink" material
	}

	std::shared_ptr<MeshAsset> MeshAsset::Create(const std::string& name, const MeshData& meshData) {
		//auto meshAsset = std::make_shared<MeshAsset>(name, meshData);
		auto meshAsset = std::make_shared<MeshAsset>(name);
		Application::Get().GetAssetRegistry().AddMesh(meshAsset);
		return meshAsset;
	}

	MeshAsset::MeshAsset(const std::string& filePath) {
		d3dUtil::GeometryGenerator geoGen;
		d3dUtil::GeometryGenerator::MeshData box = geoGen.CreateBox(1.5f, 1.5f, 1.5f, 0);
		
		std::vector<Vertex> vertices(box.Vertices.size());
		for (size_t i = 0; i < box.Vertices.size(); ++i) {
			vertices[i].Position = box.Vertices[i].Position;
			vertices[i].Normal = box.Vertices[i].Normal;
			vertices[i].Tangent = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
			vertices[i].UV = DirectX::XMFLOAT2(0.0f, 0.0f);
		}

		MeshData meshData;
		meshData.VertexData = vertices.data();
		meshData.VertexSize = (uint32_t)(vertices.size() * sizeof(Vertex));
		meshData.VertexStride = sizeof(Vertex);
		meshData.IndexData = box.Indices32.data();
		meshData.IndexSize = (uint32_t)(box.Indices32.size() * sizeof(uint32_t));

		m_Allocation = RenderCommand::AllocateMesh(meshData);

		SubmeshGeometry sm;
		sm.BaseVertexLocation = 0;
		sm.IndexCount = box.Indices32.size();
		sm.StartIndexLocation = 0;
		m_Submeshes.push_back(sm);

		SubmeshInstance instance;
		//instance.LocalTransform = MathHelper::Identity4x4();
		instance.LocalTransform = math::Mat4::Identity();
		instance.MaterialIndex = 0;
		MaterialData mData{};
		//instance.Material = MaterialAsset::Create("baseMaterial", mData);
		instance.SubmeshIndex = 0;
		instance.Name = "base";
		m_SubmeshInstances.push_back(instance);
	}
}
