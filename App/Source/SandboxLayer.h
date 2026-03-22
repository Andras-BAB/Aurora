#pragma once

#include "Aurora.h"

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

	private:
		bool m_IsImGuiDemoVisible = false;

		std::shared_ptr<Aurora::MeshAsset> m_MeshAsset;
		std::shared_ptr<Aurora::Scene> m_Scene;
		std::shared_ptr<Aurora::SceneRenderer> m_SceneRenderer;

		Aurora::PerspectiveCameraController m_CameraController;
	};

}
