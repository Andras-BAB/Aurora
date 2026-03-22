#include "SandboxLayer.h"

#include "../../Aurora/Vendor/imgui/imgui.h"
#include "Core/Application.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer3D.h"

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

		Aurora::Entity entity = m_Scene->CreateEntity("testEntity");
		Aurora::MeshComponent& component = entity.AddComponent<Aurora::MeshComponent>();
		Aurora::RelationshipComponent& rc = entity.AddComponent<Aurora::RelationshipComponent>();

		Aurora::Entity entity2 = m_Scene->CreateEntity("testEntity2");
		Aurora::MeshComponent& component2 = entity2.AddComponent<Aurora::MeshComponent>();
		Aurora::RelationshipComponent& rc2 = entity2.AddComponent<Aurora::RelationshipComponent>();
		Aurora::TransformComponent& transform = entity2.GetComponent<Aurora::TransformComponent>();
		transform.Translation = { 2.0f, 0.0f, 0.0f };

		rc.FirstChild = entity2;
		rc2.Parent = entity;

		Aurora::MeshData boxData{};
		m_MeshAsset = Aurora::MeshAsset::Create("box", boxData);

		std::vector<std::shared_ptr<Aurora::MaterialAsset>> materials;
		materials.push_back(std::make_shared<Aurora::MaterialAsset>("Base"));

		std::vector<std::shared_ptr<Aurora::MaterialAsset>> materials2;
		std::shared_ptr<Aurora::MaterialAsset> mat2 = std::make_shared<Aurora::MaterialAsset>("Base2");
		mat2->SetDiffuseColor({ 1.0f, 0.0f, 0.0f, 1.0f });
		materials2.push_back(mat2);

		component.Mesh = Aurora::MeshInstance::Create(m_MeshAsset, materials);
		component2.Mesh = Aurora::MeshInstance::Create(m_MeshAsset, materials2);
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

	//bool SandboxLayer::OnKeyPress(Aurora::KeyPressedEvent& e) {
	//	if(e.GetKeyCode() == Aurora::Key::I) {
	//		m_IsImGuiDemoVisible = !m_IsImGuiDemoVisible;
	//	}
	//	return false;
	//}
	
}
