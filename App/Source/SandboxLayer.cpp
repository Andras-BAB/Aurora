#include "SandboxLayer.h"

#include "../../Aurora/Vendor/imgui/imgui.h"
#include "Core/Application.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer3D.h"
#include "Scene/ModelLoader.h"

namespace Sandbox {

	SandboxLayer::SandboxLayer() : Layer("AppLayer") {
	}

	void SandboxLayer::OnAttach() {
		m_Scene = std::make_shared<Aurora::Scene>();

		m_SceneRenderer = std::make_shared<Aurora::SceneRenderer>();

		Aurora::Entity cameraEntity = m_Scene->CreateEntity("cameraEntity");
		Aurora::CameraComponent& cameraC = cameraEntity.AddComponent<Aurora::CameraComponent>();
		m_CameraController.SetTarget(&cameraC.Camera);
		m_CameraController.OnResize((float)Aurora::Application::Get().GetWindow().GetWidth(), (float)Aurora::Application::Get().GetWindow().GetHeight());

		m_TestTexture = std::make_shared<Aurora::DirectX12Texture2D>("textures/BambooStructure_01_T_A.png");

		m_CupboardTextures.push_back(std::make_shared<Aurora::DirectX12Texture2D>("textures/TrimWooden_02_T_A.png"));
		m_CupboardTextures.push_back(std::make_shared<Aurora::DirectX12Texture2D>("textures/Door_01_T_A.png"));
		m_CupboardTextures.push_back(std::make_shared<Aurora::DirectX12Texture2D>("textures/Window_01_T_A.png"));
		
		auto bambooPrefab = Aurora::ModelLoader::Load("models/BambooStructure_Frame_01_Mesh.fbx");
		if (bambooPrefab) {
			bambooGate = m_Scene->InstantiatePrefab(bambooPrefab, Aurora::Entity{});

			auto& transform = bambooGate.GetComponent<Aurora::TransformComponent>();
			transform.Translation = { 0.0f, 0.0f, 0.0f };
			transform.Scale = { 0.01f, 0.01f, 0.01f };

			if (m_TestTexture->IsLoaded()) {
				SetEntityTextureRecursive(bambooGate, m_TestTexture->GetHandle().Index);
			}
		}

		auto secondPrefab = Aurora::ModelLoader::Load("models/Cupboard_200x50x100_01_Mesh.fbx");

		if (secondPrefab) {
			Aurora::Entity secondEntity = m_Scene->InstantiatePrefab(secondPrefab, Aurora::Entity{});

			auto& transform2 = secondEntity.GetComponent<Aurora::TransformComponent>();
			transform2.Translation = { 5.0f, 0.0f, 0.0f };
			transform2.Scale = { 0.01f, 0.01f, 0.01f };

			std::vector<uint32_t> cupboardIndices;
			bool allLoaded = true;
			for (auto& tex : m_CupboardTextures) {
				if (tex && tex->IsLoaded()) {
					cupboardIndices.push_back(tex->GetHandle().Index);
				} else {
					allLoaded = false;
				}
			}

			if (allLoaded && !cupboardIndices.empty()) {
				uint32_t textureCounter = 0;
				SetEntityMultipleTexturesRecursive(secondEntity, cupboardIndices, textureCounter);
			}
		}
	}

	void SandboxLayer::OnDetach() {

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
		math::Vec3& transformBamboo = bambooGate.GetComponent<Aurora::TransformComponent>().Translation;
		ImGui::Begin("Edit");
		ImGui::DragFloat3("pos1", transformBamboo.v.m128_f32, 0.1, -5.f, 5.f);

		static float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		if (ImGui::ColorEdit4("Bamboo Color", color)) {
			SetEntityColorRecursive(bambooGate, { color[0], color[1], color[2], color[3] });
		}
		ImGui::End();

		return;
		Aurora::Entity testEntity = m_Scene->FindEntityByName("testEntity");
		math::Vec3& transform = testEntity.GetComponent<Aurora::TransformComponent>().Translation;
		math::Vec3& transformRot = testEntity.GetComponent<Aurora::TransformComponent>().Rotation;

		Aurora::Entity testEntity2 = m_Scene->FindEntityByName("testEntity2");
		math::Vec3& transform2 = testEntity2.GetComponent<Aurora::TransformComponent>().Translation;
		math::Vec3& transform2Rot = testEntity2.GetComponent<Aurora::TransformComponent>().Rotation;

		ImGui::Begin("Transform");
		ImGui::DragFloat3("pos1", transform.v.m128_f32, 0.1, -5.f, 5.f);
		ImGui::DragFloat3("rot1", transformRot.v.m128_f32, 0.1, -3.3f, 3.3f);
		ImGui::DragFloat3("pos2", transform2.v.m128_f32, 0.1, -5.f, 5.f);
		ImGui::DragFloat3("rot2", transform2Rot.v.m128_f32, 0.1, -3.3f, 3.3f);
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
						auto material = meshComp.Mesh->GetMaterial(submeshInst.MaterialIndex);
						if (material) {
							material->SetDiffuseColor(color);
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

	void SandboxLayer::SetEntityTextureRecursive(Aurora::Entity entity, uint32_t textureIndex) {
		if (entity.HasComponent<Aurora::MeshComponent>()) {
			auto& meshComp = entity.GetComponent<Aurora::MeshComponent>();
			if (meshComp.Mesh) {
				auto asset = meshComp.Mesh->GetMesh();
				if (asset) {
					for (const auto& submeshInst : asset->GetSubmeshInstances()) {
						auto material = meshComp.Mesh->GetMaterial(submeshInst.MaterialIndex);
						if (material) {
							material->SetDiffuseMapIndex(textureIndex);

							//material->SetDiffuseColor({ 1.0f, 1.0f, 1.0f, 1.0f });
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

				SetEntityTextureRecursive(child, textureIndex);

				childHandle = child.GetComponent<Aurora::RelationshipComponent>().NextSibling;
			}
		}
	}

	void SandboxLayer::SetEntityMultipleTexturesRecursive(Aurora::Entity entity, const std::vector<uint32_t>& textureIndices, uint32_t& currentTexIdx) {
		if (textureIndices.empty()) return;

		if (entity.HasComponent<Aurora::MeshComponent>()) {
			auto& meshComp = entity.GetComponent<Aurora::MeshComponent>();
			if (meshComp.Mesh) {
				auto asset = meshComp.Mesh->GetMesh();
				if (asset) {
					for (const auto& submeshInst : asset->GetSubmeshInstances()) {
						auto originalMaterial = meshComp.Mesh->GetMaterial(submeshInst.MaterialIndex);
						if (originalMaterial) {
							uint32_t texIdx = textureIndices[currentTexIdx % textureIndices.size()];

							std::string newMatName = "_Clone_" + std::to_string(currentTexIdx);
							auto clonedMaterial = Aurora::MaterialAsset::Create(newMatName, originalMaterial->GetData());

							clonedMaterial->SetDiffuseMapIndex(texIdx);

							meshComp.Mesh->SetMaterial(submeshInst.MaterialIndex, clonedMaterial);

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

				SetEntityMultipleTexturesRecursive(child, textureIndices, currentTexIdx);

				childHandle = child.GetComponent<Aurora::RelationshipComponent>().NextSibling;
			}
		}
	}
}
