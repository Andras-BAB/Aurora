#include "aupch.h"

#include "SceneSerializer.h"
#include "Components.h"
#include "Entity.h"

#include "Aurora/Utils/YAMLConverter.h"

#include <yaml-cpp/yaml.h>
#include <filesystem>
#include <fstream>

#include "ModelLoader.h"
#include "Aurora/Core/Application.h"

namespace Aurora {
	static void SerializeEntity(YAML::Emitter& out, Entity entity, Scene* scene) {
		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

		if (entity.HasComponent<TagComponent>()) {
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap;
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<TransformComponent>()) {
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;
			auto& tc = entity.GetComponent<TransformComponent>();

			out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
			out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << tc.Scale;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<RelationshipComponent>()) {
			out << YAML::Key << "RelationshipComponent";
			out << YAML::BeginMap;
			auto& rel = entity.GetComponent<RelationshipComponent>();

			uint64_t parentUUID = 0;
			if (rel.HasParent()) {
				parentUUID = Entity{ rel.Parent, scene }.GetUUID();
			}
			out << YAML::Key << "Parent" << YAML::Value << parentUUID;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<CameraComponent>()) {
			auto& cc = entity.GetComponent<CameraComponent>();

			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;

			out << YAML::Key << "Position" << YAML::Value << cc.Camera.GetPosition();
			out << YAML::Key << "Right" << YAML::Value << cc.Camera.GetRight();
			out << YAML::Key << "Up" << YAML::Value << cc.Camera.GetUp();
			out << YAML::Key << "Look" << YAML::Value << cc.Camera.GetLook();
			out << YAML::Key << "WorldUp" << YAML::Value << cc.Camera.GetWorldUp();

			out << YAML::Key << "NearZ" << YAML::Value << cc.Camera.GetNearZ();
			out << YAML::Key << "FarZ" << YAML::Value << cc.Camera.GetFarZ();
			out << YAML::Key << "AspectRatio" << YAML::Value << cc.Camera.GetAspect();
			out << YAML::Key << "FovX" << YAML::Value << cc.Camera.GetFovX();
			out << YAML::Key << "FovY" << YAML::Value << cc.Camera.GetFovY();

			out << YAML::Key << "Primary" << YAML::Value << cc.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cc.FixedAspectRatio;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<MeshComponent>()) {
			out << YAML::Key << "MeshComponent";
			out << YAML::BeginMap;

			auto& meshComp = entity.GetComponent<MeshComponent>();
			if (meshComp.Mesh && meshComp.Mesh->GetMesh()) {
				auto meshAsset = meshComp.Mesh->GetMesh();

				out << YAML::Key << "MeshAssetUUID" << YAML::Value << (uint64_t)meshAsset->GetUUID();

				out << YAML::Key << "Materials" << YAML::Value << YAML::BeginSeq;

				auto& submeshes = meshAsset->GetSubmeshInstances();
				std::vector<uint32_t> processedMaterials;

				for (const auto& submesh : submeshes) {
					uint32_t matIndex = submesh.MaterialIndex;

					if (std::find(processedMaterials.begin(), processedMaterials.end(), matIndex) != processedMaterials.end()) {
						continue;
					}
					processedMaterials.push_back(matIndex);

					auto matInstance = meshComp.Mesh->GetMaterialInstance(matIndex);
					if (matInstance) {
						out << YAML::BeginMap;
						out << YAML::Key << "MaterialIndex" << YAML::Value << matIndex;

						if (matInstance->HasOverride(MaterialOverride::DiffuseAlbedo))
							out << YAML::Key << "DiffuseAlbedo" << YAML::Value << matInstance->GetData().DiffuseAlbedo;

						if (matInstance->HasOverride(MaterialOverride::Roughness))
							out << YAML::Key << "Roughness" << YAML::Value << matInstance->GetData().Roughness;

						if (matInstance->HasOverride(MaterialOverride::DiffuseMapIndex)) {
							auto tex = matInstance->GetData().DiffuseMap;
							if (tex && tex->IsLoaded()) {
								out << YAML::Key << "DiffuseMapPath" << YAML::Value << tex->GetPath();
							}
						}
						out << YAML::EndMap;
					}
				}
				out << YAML::EndSeq;
			}
			out << YAML::EndMap;
		}

		out << YAML::EndMap;
	}

	SceneSerializer::SceneSerializer(const std::shared_ptr<Scene>& scene) : m_Scene(scene) {
	}

	bool SceneSerializer::Serialize(const std::string& path) {
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		auto view = m_Scene->GetRegistry().view<IDComponent>();
		for (auto entityID : view) {
			Entity entity = { entityID, m_Scene.get() };
			if (!entity) continue;

			SerializeEntity(out, entity, m_Scene.get());
		}

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::filesystem::path filepath = path;
		if (filepath.has_parent_path()) {
			std::filesystem::create_directories(filepath.parent_path());
		}

		std::ofstream fout(path);
		fout << out.c_str();

		AU_CORE_INFO("Scene saved: {0}", path);

		return true;
	}

	bool SceneSerializer::Deserialize(const std::string& path) {
		YAML::Node data;
		try {
			data = YAML::LoadFile(path);
		} catch (YAML::ParserException& e) {
			AU_CORE_ERROR("Cannot load scene: {0} - {1}", path, e.msg);
			return false;
		}

		if (!data["Scene"]) return false;

		auto entities = data["Entities"];
		if (entities) {
			for (auto entity : entities) {
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				std::string name;
				if (auto tagComponent = entity["TagComponent"]) name = tagComponent["Tag"].as<std::string>();

				Entity deserializedEntity = m_Scene->CreateEntityWithUUID(uuid, name);

				if (auto transformComponent = entity["TransformComponent"]) {
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					tc.Translation = transformComponent["Translation"].as<math::Vec3>();
					tc.Rotation = transformComponent["Rotation"].as<math::Quat>();
					tc.Scale = transformComponent["Scale"].as<math::Vec3>();
				}

				if (auto cameraComponent = entity["CameraComponent"]) {
					auto& cc = deserializedEntity.AddComponent<CameraComponent>();
					
					cc.Camera.SetPosition(cameraComponent["Position"].as<math::Vec3>());
					cc.Camera.LookAt(cameraComponent["Position"].as<math::Vec3>(),
						cameraComponent["Look"].as<math::Vec3>(), cameraComponent["Up"].as<math::Vec3>());

					cc.Camera.SetLens(cameraComponent["FovY"].as<float>(),
						cameraComponent["AspectRatio"].as<float>(), cameraComponent["NearZ"].as<float>(), cameraComponent["FarZ"].as<float>());

					cc.Primary = cameraComponent["Primary"].as<bool>();
					cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
				}

				if (auto meshComponentNode = entity["MeshComponent"]) {
					auto& mc = deserializedEntity.AddComponent<MeshComponent>();

					uint64_t meshUUID = meshComponentNode["MeshAssetUUID"].as<uint64_t>();

					auto meshAsset = Application::Get().GetAssetRegistry().GetMesh(meshUUID);

					if (!meshAsset) {
						std::string filepath = Application::Get().GetAssetRegistry().GetAssetPath(meshUUID);

						if (!filepath.empty()) {
							AU_CORE_INFO("MeshAsset (UUID: {0}) loading from assets.yaml: {1}", meshUUID, filepath);
							ModelLoader::Load(filepath);

							meshAsset = Application::Get().GetAssetRegistry().GetMesh(meshUUID);
						} else {
							AU_CORE_WARN("Could not find filepath in assets.yaml for this UUID: {0}", meshUUID);
						}
					}

					if (meshAsset) {
						// TODO: save MaterialAssets into .aumat files and MaterialInstances to the scene file

						auto defaultBaseMaterial = MaterialAsset::Create("DefaultMaterial", MaterialData());
						std::vector<std::shared_ptr<MaterialAsset>> baseMaterials(meshAsset->GetSubmeshInstances().size(), defaultBaseMaterial);

						mc.Mesh = MeshInstance::Create(meshAsset, baseMaterials);

						auto materialsNode = meshComponentNode["Materials"];
						if (materialsNode && mc.Mesh) {
							for (auto matNode : materialsNode) {
								uint32_t matIndex = matNode["MaterialIndex"].as<uint32_t>();
								auto matInstance = mc.Mesh->GetMaterialInstance(matIndex);

								if (matInstance) {
									if (matNode["DiffuseAlbedo"])
										matInstance->SetDiffuseColor(matNode["DiffuseAlbedo"].as<math::Vec4>());

									if (matNode["Roughness"])
										matInstance->SetRoughness(matNode["Roughness"].as<float>());

									if (matNode["DiffuseMapPath"]) {
										std::string texPath = matNode["DiffuseMapPath"].as<std::string>();
										auto texture = ITexture2D::Create(texPath);
										matInstance->SetDiffuseMap(texture);
									}
								}
							}
						}
					}
				}
			}

			for (auto entity : entities) {
				uint64_t childUUID = entity["Entity"].as<uint64_t>();
				Entity childEntity = m_Scene->GetEntityByUUID(childUUID);

				auto relComponent = entity["RelationshipComponent"];
				if (relComponent) {
					uint64_t parentUUID = relComponent["Parent"].as<uint64_t>();

					if (parentUUID != 0) {
						Entity parentEntity = m_Scene->GetEntityByUUID(parentUUID);
						if (parentEntity) {
							childEntity.SetParent(parentEntity);
						}
					}
				}
			}
		}

		AU_CORE_INFO("Scene loaded: {0}", path);

		return true;
	}
}
