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

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
			m_SelectedEntity = {};
		}
		ImGui::End();

		ImGui::Begin("Properties");

		if (m_SelectedEntity) {
			DrawComponents(m_SelectedEntity);
		} else {
			ImGui::Text("Select an entity to view its properties.");
		}

		ImGui::End();

		//ImGui::ShowDemoWindow(&m_ShowImGuiDemo);
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

		// more components
	}
}
