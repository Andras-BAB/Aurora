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

						auto effective = matInstance->GetEffectiveData();
						out << YAML::Key << "DiffuseAlbedo" << YAML::Value << effective.DiffuseAlbedo;
						out << YAML::Key << "Roughness" << YAML::Value << effective.Roughness;

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

		if (entity.HasComponent<PointLightComponent>()) {
			auto& plc = entity.GetComponent<PointLightComponent>();

			out << YAML::Key << "PointLightComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "Color" << YAML::Value << plc.Color;
			out << YAML::Key << "Intensity" << YAML::Value << plc.Intensity;
			out << YAML::Key << "Radius" << YAML::Value << plc.Radius;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<DirectionalLightComponent>()) {
			auto& dlc = entity.GetComponent<DirectionalLightComponent>();

			out << YAML::Key << "DirectionalLightComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "Direction" << YAML::Value << dlc.Direction;
			out << YAML::Key << "Strength" << YAML::Value << dlc.Strength;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<ParticleEmitterComponent>()) {
			auto& pe = entity.GetComponent<ParticleEmitterComponent>();

			out << YAML::Key << "ParticleEmitterComponent";
			out << YAML::BeginMap;

			out << YAML::Key << "MaxParticles" << YAML::Value << pe.MaxParticles;
			out << YAML::Key << "IsActive" << YAML::Value << pe.IsActive;
			out << YAML::Key << "Looping" << YAML::Value << pe.Looping;
			out << YAML::Key << "BlendMode" << YAML::Value << (uint32_t)pe.BlendMode;

			out << YAML::Key << "Velocity" << YAML::Value << pe.Velocity;
			out << YAML::Key << "VelocityVariation" << YAML::Value << pe.VelocityVariation;
			out << YAML::Key << "VelocityRadial" << YAML::Value << pe.VelocityRadial;

			out << YAML::Key << "LifeTime" << YAML::Value << pe.LifeTime;
			out << YAML::Key << "LifeTimeVariation" << YAML::Value << pe.LifeTimeVariation;

			out << YAML::Key << "ColorBegin" << YAML::Value << pe.ColorBegin;
			out << YAML::Key << "ColorEnd" << YAML::Value << pe.ColorEnd;

			out << YAML::Key << "SizeBegin" << YAML::Value << pe.SizeBegin;
			out << YAML::Key << "SizeEnd" << YAML::Value << pe.SizeEnd;

			out << YAML::Key << "SpawnExtents" << YAML::Value << pe.SpawnExtents;
			out << YAML::Key << "EmissionRate" << YAML::Value << pe.EmissionRate;

			if (pe.Texture && pe.Texture->IsLoaded()) {
				out << YAML::Key << "TexturePath" << YAML::Value << pe.Texture->GetPath();
			}

			out << YAML::Key << "Gradient" << YAML::Value << YAML::BeginSeq;
			for (auto& key : pe.Gradient.Keys) {
				out << YAML::BeginMap;
				out << YAML::Key << "Time" << YAML::Value << key.Time;
				out << YAML::Key << "Color" << YAML::Value << key.Color;
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;

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
					
					math::Vec3 cameraPosition = cameraComponent["Position"].as<math::Vec3>();
					cc.Camera.SetPosition(cameraPosition);

					cc.Camera.LookAt(cameraPosition,
						cameraPosition + cameraComponent["Look"].as<math::Vec3>(), cameraComponent["Up"].as<math::Vec3>());

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

						uint32_t maxMaterialIndex = 0;
						for (const auto& submeshInst : meshAsset->GetSubmeshInstances()) {
							maxMaterialIndex = std::max(maxMaterialIndex, submeshInst.MaterialIndex);
						}

						auto defaultBaseMaterial = MaterialAsset::Create("DefaultMaterial", MaterialData());
						std::vector<std::shared_ptr<MaterialAsset>> baseMaterials(maxMaterialIndex + 1, defaultBaseMaterial);

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

				if (auto pointLightNode = entity["PointLightComponent"]) {
					auto& plc = deserializedEntity.AddComponent<PointLightComponent>();
					plc.Color = pointLightNode["Color"].as<math::Vec3>();
					plc.Intensity = pointLightNode["Intensity"].as<float>();
					plc.Radius = pointLightNode["Radius"].as<float>();
				}

				if (auto dirLightNode = entity["DirectionalLightComponent"]) {
					auto& dlc = deserializedEntity.AddComponent<DirectionalLightComponent>();
					dlc.Direction = dirLightNode["Direction"].as<math::Vec3>();
					dlc.Strength = dirLightNode["Strength"].as<math::Vec3>();
				}

				if (auto particleNode = entity["ParticleEmitterComponent"]) {
					auto& pe = deserializedEntity.AddComponent<ParticleEmitterComponent>();

					pe.MaxParticles = particleNode["MaxParticles"].as<uint32_t>();
					pe.IsActive = particleNode["IsActive"].as<bool>();
					pe.Looping = particleNode["Looping"].as<bool>();
					pe.BlendMode = (Aurora::BlendMode)particleNode["BlendMode"].as<uint32_t>();

					pe.Velocity = particleNode["Velocity"].as<math::Vec3>();
					pe.VelocityVariation = particleNode["VelocityVariation"].as<float>();
					pe.VelocityRadial = particleNode["VelocityRadial"].as<float>();

					pe.LifeTime = particleNode["LifeTime"].as<float>();
					pe.LifeTimeVariation = particleNode["LifeTimeVariation"].as<float>();

					pe.ColorBegin = particleNode["ColorBegin"].as<math::Vec4>();
					pe.ColorEnd = particleNode["ColorEnd"].as<math::Vec4>();

					pe.SizeBegin = particleNode["SizeBegin"].as<float>();
					pe.SizeEnd = particleNode["SizeEnd"].as<float>();

					pe.SpawnExtents = particleNode["SpawnExtents"].as<math::Vec3>();
					pe.EmissionRate = particleNode["EmissionRate"].as<float>();

					if (particleNode["TexturePath"]) {
						pe.Texture = ITexture2D::Create(particleNode["TexturePath"].as<std::string>());
					}

					if (auto gradientNode = particleNode["Gradient"]) {
						pe.Gradient.Keys.clear();
						for (auto keyNode : gradientNode) {
							ColorKey key;
							key.Time = keyNode["Time"].as<float>();
							key.Color = keyNode["Color"].as<math::Vec4>();
							pe.Gradient.Keys.push_back(key);
						}
						pe.IsGradientDirty = true;
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
