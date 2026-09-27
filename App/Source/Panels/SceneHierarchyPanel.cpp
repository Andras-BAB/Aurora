#include "SceneHierarchyPanel.h"

#include "UI/UIHelper.h"
#include "Scene/ModelLoader.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <algorithm>
#include <filesystem>
#include <unordered_set>

namespace Aurora {

	namespace {
		bool IsMeshExtension(const std::string& ext) {
			static const std::unordered_set<std::string> extensions = { ".fbx", ".obj", ".gltf", ".glb" };
			return extensions.contains(ext);
		}

		bool IsTextureExtension(const std::string& ext) {
			static const std::unordered_set<std::string> extensions = { ".png", ".jpg", ".jpeg", ".tga", ".bmp" };
			return extensions.contains(ext);
		}

		std::string ToLowerExtension(const std::filesystem::path& path) {
			std::string ext = path.extension().string();
			std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return (char)std::tolower(c); });
			return ext;
		}

		std::string AssetDropTarget(const char* strId, const std::string& currentLabel) {
			std::string buttonLabel = currentLabel + "##" + strId;
			ImGui::Button(buttonLabel.c_str(), ImVec2(-1, 0));

			std::string droppedPath;
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("AURORA_ASSET_PATH")) {
					droppedPath = std::string((const char*)payload->Data);
				}
				ImGui::EndDragDropTarget();
			}
			return droppedPath;
		}
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction) {
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_FramePadding;

		if (entity.HasComponent<T>()) {
			auto& component = entity.GetComponent<T>();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();

			bool removeComponent = false;
			if (ImGui::BeginPopupContextItem()) {
				if (ImGui::MenuItem("Remove Component")) removeComponent = true;
				ImGui::EndPopup();
			}

			if (open) {
				if (ImGui::BeginTable("##component_table", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp)) {
					ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 125.0f);
					ImGui::TableSetupColumn("Widget", ImGuiTableColumnFlags_WidthStretch);

					uiFunction(component);

					ImGui::EndTable();
				}
				ImGui::TreePop();
			}

			if (removeComponent) entity.RemoveComponent<T>();
		}
	}

	SceneHierarchyPanel::SceneHierarchyPanel(const std::shared_ptr<Scene>& scene) {
		SetContext(scene);
	}

	void SceneHierarchyPanel::SetContext(const std::shared_ptr<Scene>& scene) {
		m_Scene = scene;
	}

	void SceneHierarchyPanel::OnImGuiRender() {
		ImGui::Begin("Scene Hierarchy");

		ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F, ImGuiInputFlags_Tooltip);
		ImGui::PushItemFlag(ImGuiItemFlags_NoNavDefaultFocus, true);
		if (ImGui::InputTextWithHint("##Filter", "Search, -exclude", m_HierarchyFilter.InputBuf, IM_COUNTOF(m_HierarchyFilter.InputBuf), ImGuiInputTextFlags_EscapeClearsAll))
			m_HierarchyFilter.Build();
		ImGui::PopItemFlag();

		ImGui::Spacing();

		m_EntityToBeDeleted = {};

		auto view = m_Scene->GetAllEntitiesWith<IDComponent>();

		if (ImGui::BeginTable("##hierarchy_bg", 1, ImGuiTableFlags_RowBg)) {
			for (auto entity : view) {
				Entity e{ entity, m_Scene.get() };

				if (e.HasComponent<RelationshipComponent>()) {
					if (e.GetComponent<RelationshipComponent>().Parent == entt::null && HasMatchingDescendant(e))
						DrawEntityNode(e);
				} else {
					if (HasMatchingDescendant(e))
						DrawEntityNode(e);
				}
			}
			ImGui::EndTable();
		}

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) {
			m_SelectedEntity = {};
		}

		if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems)) {
			if (ImGui::MenuItem("Create Empty Entity")) {
				m_Scene->CreateEntity("Empty Entity");
			}
			ImGui::EndPopup();
		}
		ImGui::End();

		if (m_EntityToBeDeleted) {
			m_Scene->DestroyEntity(m_EntityToBeDeleted);
			if (m_SelectedEntity == m_EntityToBeDeleted) {
				m_SelectedEntity = {};
			}
			m_EntityToBeDeleted = {};
		}

		ImGui::Begin("Properties");
		if (m_SelectedEntity && m_Scene->GetRegistry().valid(m_SelectedEntity)) {
			DrawComponents(m_SelectedEntity);
		}
		ImGui::End();
	}

	Entity SceneHierarchyPanel::GetSelectedEntity() const {
		return m_SelectedEntity;
	}

	void SceneHierarchyPanel::SetSelectedEntity(Entity entity) {
		m_SelectedEntity = entity;
	}

	template <typename T>
	void SceneHierarchyPanel::DisplayAddComponentEntry(const std::string& entryName) {
		if (!m_SelectedEntity.HasComponent<T>()) {
			if (ImGui::MenuItem(entryName.c_str())) {
				m_SelectedEntity.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity) {
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		ImGui::PushID((uint32_t)entity);

		if (entity == m_EntityToExpand) {
			ImGui::SetNextItemOpen(true);
			m_EntityToExpand = {};
		}

		ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0);
		flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		flags |= ImGuiTreeNodeFlags_SpanFullWidth;
		flags |= ImGuiTreeNodeFlags_DrawLinesToNodes;
		flags |= ImGuiTreeNodeFlags_NavLeftJumpsToParent;

		bool hasChildren = false;
		if (entity.HasComponent<RelationshipComponent>()) {
			hasChildren = entity.GetComponent<RelationshipComponent>().FirstChild != entt::null;
		}
		if (!hasChildren) flags |= ImGuiTreeNodeFlags_Leaf;

		if (m_HierarchyFilter.IsActive()) {
			flags |= ImGuiTreeNodeFlags_DefaultOpen;
		}

		bool opened = ImGui::TreeNodeEx("", flags, "%s", tag.c_str());

		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
			m_SelectedEntity = entity;
			if (entity.HasComponent<TransformComponent>()) {
				m_SelectedEntityEuler = math::Quat::ToEuler(entity.GetComponent<TransformComponent>().Rotation);
			}
		}

		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Delete Entity")) {
				m_EntityToBeDeleted = entity;
			}
			if (ImGui::MenuItem("Create Child Entity")) {
				Entity child = m_Scene->CreateEntity("New Child");
				child.SetParent(entity);

				m_EntityToExpand = entity;
				m_SelectedEntity = child;
			}
			ImGui::EndPopup();
		}

		if (opened) {
			if (hasChildren) {
				entt::entity childHandle = entity.GetComponent<RelationshipComponent>().FirstChild;
				while (childHandle != entt::null) {
					Entity child{ childHandle, m_Scene.get() };

					if (HasMatchingDescendant(child)) {
						DrawEntityNode(child);
					}

					childHandle = child.GetComponent<RelationshipComponent>().NextSibling;
				}
			}
			ImGui::TreePop();
		}

		ImGui::PopID();
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity) {
		auto DrawPropertyRow = [](const char* label, std::function<void()> drawWidget) {
			ImGui::TableNextRow();

			ImGui::TableNextColumn();
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted(label);

			ImGui::TableNextColumn();
			ImGui::SetNextItemWidth(-FLT_MIN);
			drawWidget();
			};

		if (entity.HasComponent<TagComponent>()) {
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());

			if (ImGui::InputText("##Tag", buffer, sizeof(buffer))) {
				tag = std::string(buffer);
			}
		}

		DrawComponent<TransformComponent>("Transform", entity, [&](auto& component) {
			DrawPropertyRow("Translation", [&]() {
				UI::InfiniteDragFloat3("##Translation", &component.Translation.x, 0.1f);
			});

			DrawPropertyRow("Rotation", [&]() {
				math::Vec3 eulerDegrees;
				eulerDegrees.x = m_SelectedEntityEuler.x * (180.0f / math::PI);
				eulerDegrees.y = m_SelectedEntityEuler.y * (180.0f / math::PI);
				eulerDegrees.z = m_SelectedEntityEuler.z * (180.0f / math::PI);

				if (UI::InfiniteDragFloat3("##Rotation", &eulerDegrees.x, 1.0f)) {
					m_SelectedEntityEuler.x = eulerDegrees.x * (math::PI / 180.0f);
					m_SelectedEntityEuler.y = eulerDegrees.y * (math::PI / 180.0f);
					m_SelectedEntityEuler.z = eulerDegrees.z * (math::PI / 180.0f);
					component.Rotation = math::Quat::FromEuler(m_SelectedEntityEuler);
				}
			});

			DrawPropertyRow("Scale", [&]() {
				UI::InfiniteDragFloat3("##Scale", &component.Scale.x, 0.1f);
			});
		});

		DrawComponent<PointLightComponent>("Point Light", entity, [&](auto& component) {
			DrawPropertyRow("Color", [&]() {
				ImGui::ColorEdit3("##Color", &component.Color.x);
			});
			DrawPropertyRow("Intensity", [&]() {
				ImGui::DragFloat("##Intensity", &component.Intensity, 0.1f, 0.0f, 10.0f);
			});
			DrawPropertyRow("Radius", [&]() {
				ImGui::DragFloat("##Radius", &component.Radius, 0.1f, 0.0f, 50.0f);
			});
		});

		DrawComponent<DirectionalLightComponent>("Directional Light", entity, [&](auto& component) {
			DrawPropertyRow("Direction", [&]() {
				ImGui::SliderFloat3("##Color", &component.Direction.x, 0.0f, math::TWO_PI);
			});
			DrawPropertyRow("Strength", [&]() {
				ImGui::ColorEdit3("##Strength", &component.Strength.x);
			});
		});

		DrawComponent<ParticleEmitterComponent>("Particle Emitter", entity, [&](auto& component) {
			auto& emitter = component;

			DrawPropertyRow("Active", [&]() {
				ImGui::Checkbox("##Active", &emitter.IsActive);
			});

			DrawPropertyRow("Looping", [&]() {
				ImGui::Checkbox("##Looping", &emitter.Looping);
			});

			// TODO: if changing max particle count we have to recreate the buffer
			// DrawPropertyRow("Max Particles", [&]() { ImGui::DragInt("##MaxParticles", (int*)&emitter.MaxParticles, 100, 100, 100000); });

			DrawPropertyRow("Blend Mode", [&]() {
				const char* blendModeStrings[] = { "Opaque", "AlphaBlend", "Additive" };
				int currentBlendModeIdx = static_cast<int>(emitter.BlendMode);

				if (ImGui::Combo("##BlendMode", &currentBlendModeIdx, blendModeStrings, IM_ARRAYSIZE(blendModeStrings))) {
					emitter.BlendMode = static_cast<Aurora::BlendMode>(currentBlendModeIdx);
				}
			});

			DrawPropertyRow("Texture", [&]() {
				std::string texLabel = "None (drop a texture)";
				if (emitter.Texture && emitter.Texture->IsLoaded()) {
					texLabel = std::filesystem::path(emitter.Texture->GetPath()).filename().string();
				}

				std::string dropped = AssetDropTarget("ParticleTextureSlot", texLabel);
				if (dropped.empty()) return;

				if (IsTextureExtension(ToLowerExtension(dropped))) {
					emitter.Texture = ITexture2D::Create(dropped);
				} else {
					AU_CORE_WARN("Not a recognized texture file: {0}", dropped);
				}
			});

			DrawPropertyRow("Emission Rate", [&]() {
				ImGui::DragFloat("##EmissionRate", &emitter.EmissionRate, 10.0f, 0.0f, 10000.0f, "%.1f particles/s");
			});

			DrawPropertyRow("Spawn Extents", [&]() {
				ImGui::DragFloat3("##SpawnExtents", &emitter.SpawnExtents.x, 0.1f, 0.0f, 50.0f);
			});

			DrawPropertyRow("Velocity", [&]() {
				UI::InfiniteDragFloat3("##Velocity", &emitter.Velocity.x, 0.1f);
			});

			DrawPropertyRow("Velocity Variation", [&]() {
				ImGui::DragFloat("##VelocityVariation", &emitter.VelocityVariation, 0.1f, 0.0f, 10.0f);
			});

			DrawPropertyRow("Radial Velocity", [&]() {
				ImGui::DragFloat("##RadialVelocity", &emitter.VelocityRadial, 0.1f, -50.0f, 50.0f);
			});

			DrawPropertyRow("Life Time", [&]() {
				ImGui::DragFloat("##LifeTime", &emitter.LifeTime, 0.1f, 0.1f, 10.0f);
			});

			DrawPropertyRow("Life Time Variation", [&]() {
				ImGui::DragFloat("##LifeTimeVariation", &emitter.LifeTimeVariation, 0.1f, 0.0f, 10.0f);
			});

			DrawPropertyRow("Size Begin", [&]() {
				ImGui::DragFloat("##SizeBegin", &emitter.SizeBegin, 0.05f, 0.0f, 5.0f);
			});

			DrawPropertyRow("Size End", [&]() {
				ImGui::DragFloat("##SizeEnd", &emitter.SizeEnd, 0.05f, 0.0f, 5.0f);
			});

			DrawPropertyRow("Color Gradient", [&]() {
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

				if (ImGui::Button("Add Key")) {
					if (!emitter.Gradient.Keys.empty()) {
						float newTime = std::min(emitter.Gradient.Keys.back().Time + 0.1f, 1.0f);
						emitter.Gradient.Keys.push_back({ newTime, emitter.Gradient.Keys.back().Color });
					} else {
						emitter.Gradient.Keys.push_back({ 0.0f, math::Vec4(1, 1, 1, 1) });
					}
					emitter.IsGradientDirty = true;
				}

				if (emitter.IsGradientDirty) {
					std::sort(emitter.Gradient.Keys.begin(), emitter.Gradient.Keys.end(),
						[](const Aurora::ColorKey& a, const Aurora::ColorKey& b) { return a.Time < b.Time; });
				}
			});
		});

		DrawComponent<MeshComponent>("Mesh", entity, [&](auto& component) {
			std::string meshLabel = "None (drop a mesh file)";
			if (component.Mesh && component.Mesh->GetMesh()) {
				meshLabel = std::to_string(component.Mesh->GetMesh()->GetSubmeshInstances().size()) + " submesh(es)";
			}

			DrawPropertyRow("Mesh", [&]() {
				std::string dropped = AssetDropTarget("MeshSlot", meshLabel);
				if (dropped.empty()) return;

				if (!IsMeshExtension(ToLowerExtension(dropped))) {
					AU_CORE_WARN("Not a recognized mesh file: {0}", dropped);
					return;
				}

				auto prefab = ModelLoader::Load(dropped);
				if (prefab) {
					m_Scene->AttachModelNode(entity, prefab->RootNode);
				} else {
					AU_CORE_WARN("Failed to load model: {0}", dropped);
				}
			});

			if (!component.Mesh || !component.Mesh->GetMesh()) return;

			auto meshAsset = component.Mesh->GetMesh();
			std::vector<uint32_t> processedMaterials;

			for (const auto& submesh : meshAsset->GetSubmeshInstances()) {
				uint32_t matIndex = submesh.MaterialIndex;
				if (std::find(processedMaterials.begin(), processedMaterials.end(), matIndex) != processedMaterials.end()) continue;
				processedMaterials.push_back(matIndex);

				auto matInst = component.Mesh->GetMaterialInstance(matIndex);
				if (!matInst) continue;

				ImGui::PushID((int)matIndex);
				MaterialData data = matInst->GetEffectiveData();

				std::string texLabel = "None (drop a texture)";
				if (data.DiffuseMap && data.DiffuseMap->IsLoaded()) {
					texLabel = std::filesystem::path(data.DiffuseMap->GetPath()).filename().string();
				}

				std::string diffuseRowLabel = "Material [" + std::to_string(matIndex) + "] Diffuse";
				DrawPropertyRow(diffuseRowLabel.c_str(), [&]() {
					std::string dropped = AssetDropTarget("DiffuseSlot", texLabel);
					if (dropped.empty()) return;

					if (IsTextureExtension(ToLowerExtension(dropped))) {
						matInst->SetDiffuseMap(ITexture2D::Create(dropped));
					} else {
						AU_CORE_WARN("Not a recognized texture file: {0}", dropped);
					}
				});

				DrawPropertyRow("Color", [&]() {
					math::Vec4 color = data.DiffuseAlbedo;
					if (ImGui::ColorEdit4("##DiffuseColor", &color.x)) {
						matInst->SetDiffuseColor(color);
					}
				});

				DrawPropertyRow("Roughness", [&]() {
					float roughness = data.Roughness;
					if (ImGui::DragFloat("##Roughness", &roughness, 0.01f, 0.0f, 1.0f)) {
						matInst->SetRoughness(roughness);
					}
				});

				ImGui::PopID();
			}
		});

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		ImGui::Spacing(); ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

		float buttonWidth = 150.0f;
		ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - buttonWidth) * 0.5f);

		// adding new components
		if (ImGui::Button("Add Component", ImVec2(buttonWidth, 0))) {
			ImGui::OpenPopup("AddComponent");
		}

		if (ImGui::BeginPopup("AddComponent")) {
			DisplayAddComponentEntry<PointLightComponent>("PointLight");
			DisplayAddComponentEntry<DirectionalLightComponent>("Directional Light");
			DisplayAddComponentEntry<ParticleEmitterComponent>("Particle Emitter");
			DisplayAddComponentEntry<MeshComponent>("Mesh");

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();
		ImGui::Spacing();
	}

	bool SceneHierarchyPanel::HasMatchingDescendant(Entity entity) {
		if (!m_HierarchyFilter.IsActive()) return true;

		auto& tag = entity.GetComponent<TagComponent>().Tag;
		if (m_HierarchyFilter.PassFilter(tag.c_str())) return true;

		if (entity.HasComponent<RelationshipComponent>()) {
			entt::entity childHandle = entity.GetComponent<RelationshipComponent>().FirstChild;
			while (childHandle != entt::null) {
				Entity child{ childHandle, m_Scene.get() };
				if (HasMatchingDescendant(child)) {
					return true;
				}
				childHandle = child.GetComponent<RelationshipComponent>().NextSibling;
			}
		}

		return false;
	}
}
