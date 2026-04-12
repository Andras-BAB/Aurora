#pragma once

#include "Aurora.h"
#include "Platform/DirectX/Renderer/DirectX12Texture.h"

#include "Renderer/PerspectiveCameraController.h"
#include "Renderer/SceneRenderer.h"
#include "Scene/Scene.h"

namespace Sandbox {

	class SandboxLayer : public Aurora::Layer {
	public:

		SandboxLayer();

		void OnAttach() override;
		void OnDetach() override;
		void OnEvent(Aurora::Event& e) override;
		void OnUpdate(Aurora::Timestep ts) override;
		void OnImGuiRender() override;
		
		//bool OnKeyPress(Aurora::KeyPressedEvent& e);
		bool OnWindowResize(Aurora::WindowResizeEvent& e);

		void SetEntityColorRecursive(Aurora::Entity entity, const math::Vec4& color);
		void SetEntityTextureRecursive(Aurora::Entity entity, const std::shared_ptr<Aurora::ITexture2D>& texture);
		void SetEntityMultipleTexturesRecursive(Aurora::Entity entity, const std::vector<std::shared_ptr<Aurora::ITexture2D>>& textures, uint32_t& currentTexIdx);

		std::shared_ptr<Aurora::ITexture2D> m_TestTexture;
		std::vector<std::shared_ptr<Aurora::ITexture2D>> m_CupboardTextures;

	private:
		void DrawEntityNode(Aurora::Entity entity);
		void DrawComponents(Aurora::Entity entity);

		Aurora::Entity m_SelectedEntity;
		math::Vec3 m_SelectedEntityEuler = { 0.0f, 0.0f, 0.0f };

	private:
		bool m_IsImGuiDemoVisible = false;

		std::shared_ptr<Aurora::MeshAsset> m_MeshAsset;
		std::shared_ptr<Aurora::Scene> m_Scene;
		std::shared_ptr<Aurora::SceneRenderer> m_SceneRenderer;

		Aurora::PerspectiveCameraController m_CameraController;

		Aurora::Entity bambooGate;
	};

}
