#include "SandboxLayer.h"

#include "../../Aurora/Vendor/imgui/imgui.h"
#include "Core/Application.h"
#include "Core/Timer.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer3D.h"
#include "Scene/ModelLoader.h"
#include "Scene/SceneSerializer.h"
#include "UI/UIHelper.h"

namespace Sandbox {

	SandboxLayer::SandboxLayer() : Layer("AppLayer") {
	}

	void SandboxLayer::OnAttach() {
		m_Scene = std::make_shared<Aurora::Scene>();

		m_SceneRenderer = std::make_shared<Aurora::SceneRenderer>();

		//Aurora::SceneSerializer serializer(m_Scene);
		//serializer.Deserialize("scenes/level1.auscene");

		Aurora::Entity cameraEntity = m_Scene->CreateEntity("cameraEntity");
		Aurora::CameraComponent& cameraC = cameraEntity.AddComponent<Aurora::CameraComponent>();
		m_CameraController.SetTarget(&cameraC.Camera);
		m_CameraController.OnResize((float)Aurora::Application::Get().GetWindow().GetWidth(), (float)Aurora::Application::Get().GetWindow().GetHeight());

		Aurora::Entity emitterEntity = m_Scene->CreateEntity("emitterEntity");
		emitterEntity.GetComponent<Aurora::TransformComponent>().Translation = { 0.0f, 0.5f, 0.0f };
		Aurora::ParticleEmitterComponent& emitter = emitterEntity.AddComponent<Aurora::ParticleEmitterComponent>();
		emitter.EmissionRate = 2000.f;
		emitter.SpawnExtents = math::Vec3(0.5f, 0.5f, 0.5f);
		emitter.LifeTime = 3.f;
		emitter.SizeBegin = 0.5f;
		emitter.SizeEnd = 0.4f;

		Aurora::ParticleGradient fireGradient;
		fireGradient.Keys.clear();
		fireGradient.Keys.push_back({ 0.00f, { 247.f / 255.f, 36.f / 255.f, 5.f / 255.f, 1.0f / 255.f } });
		fireGradient.Keys.push_back({ 0.06f, { 245.f / 255.f, 36.f / 255.f, 2.f / 255.f, 249.f / 255.f } });
		fireGradient.Keys.push_back({ 0.15f, { 242.f / 255.f, 1.f / 255.f, 1.f / 255.f, 0.f / 255.f } });
		fireGradient.Keys.push_back({ 0.36f, { 91.f / 255.f, 91.f / 255.f, 91.f / 255.f, 0.f / 255.f } });
		fireGradient.Keys.push_back({ 0.44f, { 97.f / 255.f, 97.f / 255.f, 97.f / 255.f, 30.f / 255.f } });
		fireGradient.Keys.push_back({ 0.87f, { 87.f / 255.f, 87.f / 255.f, 87.f / 255.f, 133.f / 255.f } });
		fireGradient.Keys.push_back({ 1.00f, { 87.f / 255.f, 87.f / 255.f, 87.f / 255.f, 0.f / 255.f } });
		emitter.Gradient = fireGradient;
		
		Aurora::Entity dirLightE = m_Scene->CreateEntity("mainLight");
		Aurora::DirectionalLightComponent& dirLight = dirLightE.AddComponent<Aurora::DirectionalLightComponent>();
		dirLight.Direction = -MathHelper::SphericalToCartesian(1.0f, 1.25f * math::PI, math::PIDIV4);
		dirLight.Strength = { 1.0f, 1.0f, 0.9f };

		Aurora::Entity pointLightE = m_Scene->CreateEntity("pointLight");
		pointLightE.GetComponent<Aurora::TransformComponent>().Translation.y = 0.5f;
		Aurora::PointLightComponent& pLight = pointLightE.AddComponent<Aurora::PointLightComponent>();
		pLight.Color = { 247.f / 255.f, 36.f / 255.f, 5.f / 255.f };
		pLight.Intensity = 10.0f;
		pLight.Radius = 10.f;

		auto firewood = Aurora::ModelLoader::Load("models/firewoods.fbx");
		if (firewood) {
			Aurora::Entity woods = m_Scene->InstantiatePrefab(firewood, Aurora::Entity{});

			auto& transform = woods.GetComponent<Aurora::TransformComponent>();
			transform.Translation = { 0.0f, 0.0f, 0.0f };
			transform.Scale = { 0.1f, 0.1f, 0.1f };

			SetEntityColorRecursive(woods, math::Vec4(30.f / 255.f, 12.f / 255.f, 1.f / 255.f, 1.f));
		}

		//Aurora::Entity pointLightE = m_Scene->CreateEntity("pointLight");
		//Aurora::PointLightComponent& pLight = pointLightE.AddComponent<Aurora::PointLightComponent>();
		//pLight.Color = { 0.1f, 0.8f, 0.8f };
		//pLight.Intensity = 10.0f;
		//pLight.Radius = 10.f;

		/*
		m_TestTexture = Aurora::ITexture2D::Create("textures/BambooStructure_01_T_A.png");
		
		m_CupboardTextures.push_back(Aurora::ITexture2D::Create("textures/TrimWooden_02_T_A.png"));
		m_CupboardTextures.push_back(Aurora::ITexture2D::Create("textures/Door_01_T_A.png"));
		m_CupboardTextures.push_back(Aurora::ITexture2D::Create("textures/Window_01_T_A.png"));
		
		auto bambooPrefab = Aurora::ModelLoader::Load("models/BambooStructure_Frame_01_Mesh.fbx");
		if (bambooPrefab) {
			bambooGate = m_Scene->InstantiatePrefab(bambooPrefab, Aurora::Entity{});

			auto& transform = bambooGate.GetComponent<Aurora::TransformComponent>();
			transform.Translation = { 0.0f, 0.0f, 0.0f };
			transform.Scale = { 0.01f, 0.01f, 0.01f };

			if (m_TestTexture->IsLoaded()) {
				SetEntityTextureRecursive(bambooGate, m_TestTexture);
			}
		}

		auto secondPrefab = Aurora::ModelLoader::Load("models/Cupboard_200x50x100_01_Mesh.fbx");

		if (secondPrefab) {
			Aurora::Entity secondEntity = m_Scene->InstantiatePrefab(secondPrefab, Aurora::Entity{});

			auto& transform2 = secondEntity.GetComponent<Aurora::TransformComponent>();
			transform2.Translation = { 5.0f, 0.0f, 0.0f };
			transform2.Scale = { 0.01f, 0.01f, 0.01f };

			std::vector<std::shared_ptr<Aurora::ITexture2D>> validCupboardTextures;
			bool allLoaded = true;
			for (auto& tex : m_CupboardTextures) {
				if (tex && tex->IsLoaded()) {
					validCupboardTextures.push_back(tex);
				} else {
					allLoaded = false;
				}
			}

			if (allLoaded && !validCupboardTextures.empty()) {
				uint32_t textureCounter = 0;
				SetEntityMultipleTexturesRecursive(secondEntity, validCupboardTextures, textureCounter);
			}
			// -------------------------
			Aurora::Entity secondEntity_ = m_Scene->InstantiatePrefab(secondPrefab, Aurora::Entity{});

			auto& transform2_ = secondEntity_.GetComponent<Aurora::TransformComponent>();
			transform2_.Translation = { 0.0f, 0.0f, 0.0f };
			transform2_.Scale = { 0.01f, 0.01f, 0.01f };

			if (allLoaded && !validCupboardTextures.empty()) {
				uint32_t textureCounter = 0;
				SetEntityMultipleTexturesRecursive(secondEntity_, validCupboardTextures, textureCounter);
			}
		}
		*/
	}

	void SandboxLayer::OnDetach() {
		Aurora::SceneSerializer serializer(m_Scene);
		serializer.Serialize("scenes/level1.auscene");
	}

	void SandboxLayer::OnEvent(Aurora::Event& e) {
		Aurora::EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Aurora::WindowResizeEvent>(BIND_EVENT_FN(SandboxLayer::OnWindowResize));

		m_CameraController.OnEvent(e);
	}

	void SandboxLayer::OnUpdate(Aurora::Timestep ts) {
		m_Scene->UpdateTransform();
		m_CameraController.OnUpdate(ts);

		m_SceneRenderer->Render(m_Scene.get());		
	}

	void SandboxLayer::OnImGuiRender() {
		ImGui::Begin("Scene Hierarchy");

		auto view = m_Scene->GetRegistry().view<Aurora::IDComponent>();

		for (auto entityID : view) {
			Aurora::Entity entity{ entityID, m_Scene.get() };

			bool isRoot = true;
			if (m_Scene->GetRegistry().all_of<Aurora::RelationshipComponent>(entityID)) {
				isRoot = !m_Scene->GetRegistry().get<Aurora::RelationshipComponent>(entityID).HasParent();
			}

			if (isRoot) {
				DrawEntityNode(entity);
			}
		}

		//if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
		//	m_SelectedEntity = {};
		//}
		ImGui::End();

		ImGui::Begin("Properties");

		if (m_SelectedEntity) {
			DrawComponents(m_SelectedEntity);
		} else {
			ImGui::Text("Select an entity to view its properties.");
		}

		ImGui::End();

		//ImGui::ShowDemoWindow(&m_IsImGuiDemoVisible);
	}

	bool SandboxLayer::OnWindowResize(Aurora::WindowResizeEvent& e) {
		Aurora::RenderCommand::SetViewport(0, 0, e.GetWidth(), e.GetHeight());
		Aurora::RenderCommand::SetScissors(0, 0, e.GetWidth(), e.GetHeight());
		return false;
	}

	void SandboxLayer::SetEntityColorRecursive(Aurora::Entity entity, const math::Vec4& color) {
		if (entity.HasComponent<Aurora::MeshComponent>()) {
			auto& meshComp = entity.GetComponent<Aurora::MeshComponent>();
			if (meshComp.Mesh) {
				auto asset = meshComp.Mesh->GetMesh();
				if (asset) {
					for (const auto& submeshInst : asset->GetSubmeshInstances()) {
						auto materialInst = meshComp.Mesh->GetMaterialInstance(submeshInst.MaterialIndex);
						if (materialInst) {
							materialInst->SetDiffuseColor(color);
						}
					}
				}
			}
		}

		if (entity.HasComponent<Aurora::RelationshipComponent>()) {
			auto& rel = entity.GetComponent<Aurora::RelationshipComponent>();
			entt::entity childHandle = rel.FirstChild;
			while (childHandle != entt::null) {
				Aurora::Entity child(childHandle, m_Scene.get());
				SetEntityColorRecursive(child, color);
				childHandle = child.GetComponent<Aurora::RelationshipComponent>().NextSibling;
			}
		}
	}

	void SandboxLayer::SetEntityTextureRecursive(Aurora::Entity entity, const std::shared_ptr<Aurora::ITexture2D>& texture) {
		if (entity.HasComponent<Aurora::MeshComponent>()) {
			auto& meshComp = entity.GetComponent<Aurora::MeshComponent>();
			if (meshComp.Mesh) {
				auto asset = meshComp.Mesh->GetMesh();
				if (asset) {
					for (const auto& submeshInst : asset->GetSubmeshInstances()) {
						auto materialInst = meshComp.Mesh->GetMaterialInstance(submeshInst.MaterialIndex);
						if (materialInst) {
							materialInst->SetDiffuseMap(texture);
						}
					}
				}
			}
		}

		if (entity.HasComponent<Aurora::RelationshipComponent>()) {
			auto& rel = entity.GetComponent<Aurora::RelationshipComponent>();
			entt::entity childHandle = rel.FirstChild;
			while (childHandle != entt::null) {
				Aurora::Entity child(childHandle, m_Scene.get());
				SetEntityTextureRecursive(child, texture);
				childHandle = child.GetComponent<Aurora::RelationshipComponent>().NextSibling;
			}
		}
	}

	void SandboxLayer::SetEntityMultipleTexturesRecursive(Aurora::Entity entity, const std::vector<std::shared_ptr<Aurora::ITexture2D>>& textures, uint32_t& currentTexIdx) {
		if (textures.empty()) return;

		if (entity.HasComponent<Aurora::MeshComponent>()) {
			auto& meshComp = entity.GetComponent<Aurora::MeshComponent>();
			if (meshComp.Mesh) {
				auto asset = meshComp.Mesh->GetMesh();
				if (asset) {
					for (const auto& submeshInst : asset->GetSubmeshInstances()) {
						auto materialInst = meshComp.Mesh->GetMaterialInstance(submeshInst.MaterialIndex);
						if (materialInst) {
							auto tex = textures[currentTexIdx % textures.size()];
							materialInst->SetDiffuseMap(tex);
							currentTexIdx++;
						}
					}
				}
			}
		}

		if (entity.HasComponent<Aurora::RelationshipComponent>()) {
			auto& rel = entity.GetComponent<Aurora::RelationshipComponent>();
			entt::entity childHandle = rel.FirstChild;
			while (childHandle != entt::null) {
				Aurora::Entity child(childHandle, m_Scene.get());
				SetEntityMultipleTexturesRecursive(child, textures, currentTexIdx);
				childHandle = child.GetComponent<Aurora::RelationshipComponent>().NextSibling;
			}
		}
	}

	void SandboxLayer::DrawEntityNode(Aurora::Entity entity) {
		auto& tag = entity.GetComponent<Aurora::TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		bool hasChildren = false;
		if (entity.HasComponent<Aurora::RelationshipComponent>()) {
			hasChildren = entity.GetComponent<Aurora::RelationshipComponent>().FirstChild != entt::null;
		}

		if (!hasChildren) {
			flags |= ImGuiTreeNodeFlags_Leaf;
		}

		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, "%s", tag.c_str());

		if (ImGui::IsItemClicked()) {
			m_SelectedEntity = entity;

			if (m_SelectedEntity.HasComponent<Aurora::TransformComponent>()) {
				auto& transform = m_SelectedEntity.GetComponent<Aurora::TransformComponent>();
				m_SelectedEntityEuler = math::Quat::ToEuler(transform.Rotation);
			}
		}

		if (opened) {
			if (hasChildren) {
				entt::entity childHandle = entity.GetComponent<Aurora::RelationshipComponent>().FirstChild;
				while (childHandle != entt::null) {
					Aurora::Entity child{ childHandle, m_Scene.get() };
					DrawEntityNode(child);
					childHandle = child.GetComponent<Aurora::RelationshipComponent>().NextSibling;
				}
			}
			ImGui::TreePop();
		}
	}

	void SandboxLayer::DrawComponents(Aurora::Entity entity) {
		if (entity.HasComponent<Aurora::TagComponent>()) {
			auto& tag = entity.GetComponent<Aurora::TagComponent>().Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strncpy_s(buffer, tag.c_str(), sizeof(buffer) - 1);

			if (ImGui::InputText("##Tag", buffer, sizeof(buffer))) {
				tag = std::string(buffer);
			}
		}

		ImGui::Separator();

		if (entity.HasComponent<Aurora::TransformComponent>()) {
			if (ImGui::TreeNodeEx((void*)typeid(Aurora::TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform")) {
				auto& transform = entity.GetComponent<Aurora::TransformComponent>();

				Aurora::UI::InfiniteDragFloat3("Position", &transform.Translation.x, 0.1f);

				math::Vec3 eulerDegrees;
				eulerDegrees.x = m_SelectedEntityEuler.x * (180.0f / math::PI);
				eulerDegrees.y = m_SelectedEntityEuler.y * (180.0f / math::PI);
				eulerDegrees.z = m_SelectedEntityEuler.z * (180.0f / math::PI);

				if (Aurora::UI::InfiniteDragFloat3("Rotation", &eulerDegrees.x, 0.1f)) {
					m_SelectedEntityEuler.x = eulerDegrees.x * (math::PI / 180.0f);
					m_SelectedEntityEuler.y = eulerDegrees.y * (math::PI / 180.0f);
					m_SelectedEntityEuler.z = eulerDegrees.z * (math::PI / 180.0f);

					transform.Rotation = math::Quat::FromEuler(m_SelectedEntityEuler);
				}

				Aurora::UI::InfiniteDragFloat3("Scale", &transform.Scale.x, 0.1f);

				ImGui::TreePop();
			}
		}

		ImGui::Separator();

		if (entity.HasComponent<Aurora::MeshComponent>()) {
			if (ImGui::TreeNodeEx((void*)typeid(Aurora::MeshComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Mesh Component & Materials")) {
				auto& meshComp = entity.GetComponent<Aurora::MeshComponent>();

				if (meshComp.Mesh) {
					auto asset = meshComp.Mesh->GetMesh();
					if (asset) {
						std::vector<uint32_t> processedMaterials;

						for (const auto& submeshInst : asset->GetSubmeshInstances()) {
							uint32_t matIndex = submeshInst.MaterialIndex;

							if (std::find(processedMaterials.begin(), processedMaterials.end(), matIndex) != processedMaterials.end()) {
								continue;
							}
							processedMaterials.push_back(matIndex);

							auto materialInst = meshComp.Mesh->GetMaterialInstance(matIndex);
							if (materialInst) {
								if (ImGui::TreeNodeEx((void*)(uint64_t)(matIndex + 1), ImGuiTreeNodeFlags_DefaultOpen, "Material [%d]", matIndex)) {

									//ImGui::TextDisabled("Based on MaterialAsset");
									//ImGui::Separator();

									Aurora::MaterialData data = materialInst->GetEffectiveData();

									float controlWidth = ImGui::GetContentRegionAvail().x * 0.6f;

									bool hasColorOverride = materialInst->HasOverride(Aurora::MaterialOverride::DiffuseAlbedo);

									if (hasColorOverride) {
										if (ImGui::Button("X##Color", ImVec2(20, 0))) {
											materialInst->ResetOverrides(Aurora::MaterialOverride::DiffuseAlbedo);
										}
										if (ImGui::IsItemHovered()) ImGui::SetTooltip("Reset to default!");
										ImGui::SameLine();
									} else {
										ImGui::Dummy(ImVec2(20, 0));
										ImGui::SameLine();
									}

									float color[4] = { data.DiffuseAlbedo.x, data.DiffuseAlbedo.y, data.DiffuseAlbedo.z, data.DiffuseAlbedo.w };

									ImGui::PushItemWidth(controlWidth - 28.0f);
									if (ImGui::ColorEdit4("Diffuse Color", color, ImGuiColorEditFlags_NoInputs)) {
										materialInst->SetDiffuseColor({ color[0], color[1], color[2], color[3] });
									}
									ImGui::PopItemWidth();


									bool hasRoughOverride = materialInst->HasOverride(Aurora::MaterialOverride::Roughness);

									if (hasRoughOverride) {
										if (ImGui::Button("X##Roughness", ImVec2(20, 0))) {
											materialInst->ResetOverrides(Aurora::MaterialOverride::Roughness);
										}
										if (ImGui::IsItemHovered()) ImGui::SetTooltip("Reset to default");
										ImGui::SameLine();
									} else {
										ImGui::Dummy(ImVec2(20, 0));
										ImGui::SameLine();
									}

									float roughness = data.Roughness;
									ImGui::PushItemWidth(controlWidth - 28.0f);
									if (ImGui::DragFloat("Roughness", &roughness, 0.01f, 0.0f, 1.0f)) {
										materialInst->SetRoughness(roughness);
									}
									ImGui::PopItemWidth();

									ImGui::TreePop();
								}
							}
						}
					}
				} else {
					ImGui::Text("No mesh instance attached.");
				}
				ImGui::TreePop();
			}
		}

		if (entity.HasComponent<Aurora::ParticleEmitterComponent>()) {
			if (ImGui::TreeNodeEx((void*)typeid(Aurora::ParticleEmitterComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Particle Emitter")) {

				auto& emitter = entity.GetComponent<Aurora::ParticleEmitterComponent>();

				ImGui::Checkbox("Active", &emitter.IsActive);
				//ImGui::Checkbox("Looping", &emitter.Looping);

				// TODO: if changing max particle count we have to recreate the buffer
				// ImGui::DragInt("Max Particles", (int*)&emitter.MaxParticles, 100, 100, 100000);

				const char* blendModeStrings[] = { "Opaque", "AlphaBlend", "Additive" };
				int currentBlendModeIdx = static_cast<int>(emitter.BlendMode);

				if (ImGui::Combo("Blend Mode", &currentBlendModeIdx, blendModeStrings, IM_ARRAYSIZE(blendModeStrings))) {
					emitter.BlendMode = static_cast<Aurora::BlendMode>(currentBlendModeIdx);
				}

				ImGui::DragFloat("Emission Rate", &emitter.EmissionRate, 10.0f, 0.0f, 10000.0f, "%.1f particles/s");

				ImGui::DragFloat3("Spawn radius", &emitter.SpawnExtents.x, 0.1f, 0.0f, 50.0f);

				Aurora::UI::InfiniteDragFloat3("Velocity", &emitter.Velocity.x, 0.1f);
				ImGui::DragFloat("Velocity Variation", &emitter.VelocityVariation, 0.1f, 0.0f, 10.0f);
				ImGui::DragFloat("Radial Velocity", &emitter.VelocityRadial, 0.1f, -50.0f, 50.0f);

				ImGui::DragFloat("Life Time", &emitter.LifeTime, 0.1f, 0.1f, 10.0f);

				ImGui::DragFloat("Size Begin", &emitter.SizeBegin, 0.05f, 0.0f, 5.0f);
				ImGui::DragFloat("Size End", &emitter.SizeEnd, 0.05f, 0.0f, 5.0f);

				ImGui::Separator();
				ImGui::Text("Color gradient");

				// order the keys by time if anything changed
				if (emitter.IsGradientDirty) {
					std::sort(emitter.Gradient.Keys.begin(), emitter.Gradient.Keys.end(),
						[](const Aurora::ColorKey& a, const Aurora::ColorKey& b) { return a.Time < b.Time; });
					emitter.IsGradientDirty = false;
				}

				// custom ImGui drawing for the color bar
				ImDrawList* drawList = ImGui::GetWindowDrawList();
				ImVec2 p0 = ImGui::GetCursorScreenPos();
				ImVec2 p1 = ImVec2(p0.x + ImGui::GetContentRegionAvail().x, p0.y + 24.0f); // 24 pixel high bar for the color

				// base background color (can be a simple chess board pattern)
				drawList->AddRectFilled(p0, p1, IM_COL32(30, 30, 30, 255));

				if (!emitter.Gradient.Keys.empty()) {
					// fill from 0.0 to the first key
					if (emitter.Gradient.Keys.front().Time > 0.0f) {
						auto& k = emitter.Gradient.Keys.front();
						ImU32 col = ImGui::ColorConvertFloat4ToU32(ImVec4(k.Color.x, k.Color.y, k.Color.z, k.Color.w));
						ImVec2 maxPos = ImVec2(p0.x + k.Time * (p1.x - p0.x), p1.y);
						drawList->AddRectFilled(p0, maxPos, col);
					}

					// gradients between the keys
					for (size_t i = 0; i < emitter.Gradient.Keys.size() - 1; ++i) {
						auto& k1 = emitter.Gradient.Keys[i];
						auto& k2 = emitter.Gradient.Keys[i + 1];

						ImVec2 rectMin = ImVec2(p0.x + k1.Time * (p1.x - p0.x), p0.y);
						ImVec2 rectMax = ImVec2(p0.x + k2.Time * (p1.x - p0.x), p1.y);

						ImU32 col1 = ImGui::ColorConvertFloat4ToU32(ImVec4(k1.Color.x, k1.Color.y, k1.Color.z, k1.Color.w));
						ImU32 col2 = ImGui::ColorConvertFloat4ToU32(ImVec4(k2.Color.x, k2.Color.y, k2.Color.z, k2.Color.w));

						// multiColor rect: (left_top, right_top, right_bot, left_bot)
						drawList->AddRectFilledMultiColor(rectMin, rectMax, col1, col2, col2, col1);
					}

					// fill from last key to 1.0
					if (emitter.Gradient.Keys.back().Time < 1.0f) {
						auto& k = emitter.Gradient.Keys.back();
						ImU32 col = ImGui::ColorConvertFloat4ToU32(ImVec4(k.Color.x, k.Color.y, k.Color.z, k.Color.w));
						ImVec2 minPos = ImVec2(p0.x + k.Time * (p1.x - p0.x), p0.y);
						drawList->AddRectFilled(minPos, p1, col);
					}
				}

				// need it to avoid overdrawing the color bar with the first key's time slider
				ImGui::Dummy(ImVec2(0, 28.0f));
				//ImGui::Separator();

				// editing and deleting the keys
				int keyToDelete = -1; // not to delete in the loop

				for (int i = 0; i < emitter.Gradient.Keys.size(); ++i) {
					ImGui::PushID(i);

					// time slider
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 90.0f); // dynamic width
					if (ImGui::SliderFloat("##Time", &emitter.Gradient.Keys[i].Time, 0.0f, 1.0f, "%.2f")) {
						float minTime = (i > 0) ? emitter.Gradient.Keys[i - 1].Time : 0.0f;
						float maxTime = (i < emitter.Gradient.Keys.size() - 1) ? emitter.Gradient.Keys[i + 1].Time : 1.0f;

						emitter.Gradient.Keys[i].Time = std::clamp(emitter.Gradient.Keys[i].Time, minTime, maxTime);

						emitter.IsGradientDirty = true;
					}
					ImGui::SameLine();

					// color editor
					if (ImGui::ColorEdit4("##Color", &emitter.Gradient.Keys[i].Color.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel)) {
						emitter.IsGradientDirty = true;
					}
					ImGui::SameLine();

					// red delete button
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
					if (ImGui::Button("X", ImVec2(24, 0))) {
						keyToDelete = i;
					}
					ImGui::PopStyleColor(3);

					ImGui::PopID();
				}

				// delete the key
				if (keyToDelete >= 0) {
					emitter.Gradient.Keys.erase(emitter.Gradient.Keys.begin() + keyToDelete);
					emitter.IsGradientDirty = true;
				}

				// add new key
				//if (ImGui::Button("Add Key", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
				if (ImGui::Button("Add Key")) {
					// if there is a key, then use the last key's color and time (with a little offset), if there isn't, then add a white
					if (!emitter.Gradient.Keys.empty()) {
						float newTime = std::min(emitter.Gradient.Keys.back().Time + 0.1f, 1.0f);
						emitter.Gradient.Keys.push_back({ newTime, emitter.Gradient.Keys.back().Color });
					} else {
						emitter.Gradient.Keys.push_back({ 0.0f, math::Vec4(1, 1, 1, 1) });
					}
					emitter.IsGradientDirty = true;
				}
				ImGui::TreePop();

				if (emitter.IsGradientDirty) {
					std::sort(emitter.Gradient.Keys.begin(), emitter.Gradient.Keys.end(),
						[](const Aurora::ColorKey& a, const Aurora::ColorKey& b) { return a.Time < b.Time; });
				}
			}

			
		}

		ImGui::Separator();

		if (entity.HasComponent<Aurora::PointLightComponent>()) {
			if (ImGui::TreeNodeEx((void*)typeid(Aurora::PointLightComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "PointLight")) {
				auto& [Color, Radius, Intensity] = entity.GetComponent<Aurora::PointLightComponent>();
				ImGui::ColorEdit3("Color", &Color.x);
				ImGui::DragFloat("Intensity", &Intensity, 0.1f, 0.0f, 10.0f);
				ImGui::DragFloat("Radius", &Radius, 0.1f, 0.0f, 50.0f);
			
				ImGui::TreePop();
			}
		}

		// more components
	}
}
