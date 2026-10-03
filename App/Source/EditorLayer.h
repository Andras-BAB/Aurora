#pragma once

#include "Aurora.h"
#include "Platform/DirectX/Renderer/DirectX12Texture.h"

#include "Renderer/PerspectiveCameraController.h"
#include "Renderer/SceneRenderer.h"
#include "Scene/Scene.h"

#include "../../Aurora/Vendor/imgui/imgui.h"
#include "Panels/AssetBrowserPanel.h"
#include "Panels/SceneHierarchyPanel.h"

namespace Aurora {

	class EditorLayer : public Layer {
	public:

		EditorLayer();

		void OnAttach() override;
		void OnDetach() override;
		void OnEvent(Event& e) override;
		void OnUpdate(Timestep ts) override;
		void OnImGuiRender() override;
		
		//bool OnKeyPress(Aurora::KeyPressedEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		bool m_ViewportHovered = false;

	private:
		bool m_IsImGuiDemoVisible = false;

		SceneHierarchyPanel m_HierarchyPanel;
		AssetBrowserPanel m_AssetBrowserPanel;

		std::shared_ptr<Scene> m_Scene;
		std::shared_ptr<SceneRenderer> m_SceneRenderer;

		PerspectiveCamera m_EditorCamera;
		PerspectiveCameraController m_CameraController;

		math::Vec2 m_ViewportSize;
		TextureHandle m_CurrentViewportTexture;

		std::string m_PendingScenePathToLoad;
	};

}
