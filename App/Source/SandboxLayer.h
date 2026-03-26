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
		void SetEntityTextureRecursive(Aurora::Entity entity, uint32_t textureIndex);
		void SetEntityMultipleTexturesRecursive(Aurora::Entity entity, const std::vector<uint32_t>& textureIndices, uint32_t& currentTexIdx);

		std::shared_ptr<Aurora::DirectX12Texture2D> m_TestTexture;
		std::vector<std::shared_ptr<Aurora::DirectX12Texture2D>> m_CupboardTextures;

	private:
		bool m_IsImGuiDemoVisible = false;

		std::shared_ptr<Aurora::MeshAsset> m_MeshAsset;
		std::shared_ptr<Aurora::Scene> m_Scene;
		std::shared_ptr<Aurora::SceneRenderer> m_SceneRenderer;

		Aurora::PerspectiveCameraController m_CameraController;

		Aurora::Entity bambooGate;
	};

}
