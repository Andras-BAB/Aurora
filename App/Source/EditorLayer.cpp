#include "EditorLayer.h"

#include "Core/Application.h"
#include "Core/Timer.h"
#include "Platform/DirectX/Renderer/DirectX12RenderCommand.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer3D.h"
#include "Scene/ModelLoader.h"
#include "Scene/SceneSerializer.h"
#include "Utils/PlatformUtils.h"

namespace Aurora {

	EditorLayer::EditorLayer() : Layer("AppLayer") {
	}

	void EditorLayer::OnAttach() {
		m_Scene = std::make_shared<Scene>();

		m_SceneRenderer = std::make_shared<SceneRenderer>();

		m_HierarchyPanel.SetContext(m_Scene);
	}

	void EditorLayer::OnDetach() {
		m_Scene.reset();
	}

	void EditorLayer::OnEvent(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(EditorLayer::OnWindowResize));

		m_CameraController.OnEvent(e);
	}

	void EditorLayer::OnUpdate(Timestep ts) {
		if (!m_PendingScenePathToLoad.empty()) {
			DirectX12RenderCommand::GetContext()->FlushCommandQueue();
			RenderCommand::GetRendererAPIAs<DirectX12RendererAPI>()->ClearProxies();

			m_Scene = std::make_shared<Scene>();
			SceneSerializer serializer(m_Scene);
			serializer.Deserialize(m_PendingScenePathToLoad);
			m_HierarchyPanel.SetContext(m_Scene);
			m_HierarchyPanel.SetSelectedEntity({});

			if (Entity cameraEntity = m_Scene->GetPrimaryCameraEntity()) {
				m_CameraController.SetTarget(&cameraEntity.GetComponent<CameraComponent>().Camera);
			} else {
				AU_CORE_WARN("Loaded scene has no primary camera - camera controller left unbound");
			}

			AU_CORE_INFO("Scene loaded!");

			m_PendingScenePathToLoad.clear();
		}

		m_Scene->UpdateTransform();

		m_CameraController.SetActive(m_ViewportHovered);
		m_CameraController.OnUpdate(ts);

		m_CurrentViewportTexture = m_SceneRenderer->Render(m_Scene.get(), m_ViewportSize.x, m_ViewportSize.y);
	}

	void EditorLayer::OnImGuiRender() {
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen) {
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", &dockspaceOpen, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			ImGuiID dockspace_id = ImGui::GetID("DockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		
		// end of docking section

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Save Scene", "Ctrl+S")) {
					std::string path = FileDialogs::SaveFile("Aurora Scene (*.auscene)\0*.auscene\0");
					if (!path.empty()) {
						SceneSerializer serializer(m_Scene);
						serializer.Serialize(path);
						AU_CORE_INFO("Scene saved: {0}", path);
					}
				}
				if (ImGui::MenuItem("Load Scene", "Ctrl+O")) {
					std::string path = FileDialogs::OpenFile("Aurora Scene (*.auscene)\0*.auscene\0");
					if (!path.empty()) {
						m_PendingScenePathToLoad = path;
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		m_HierarchyPanel.OnImGuiRender();
		m_AssetBrowserPanel.OnImGuiRender();

		// viewport panel
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport");

		m_ViewportHovered = ImGui::IsWindowHovered();

		// viewport in another window
		if (ImGuiViewport* viewport = ImGui::GetWindowViewport()) {
			m_CameraController.SetTargetWindow(viewport->PlatformHandle);
		} else {
			m_CameraController.SetTargetWindow(nullptr);
		}

		static bool isRightDragging = false;
		if (m_ViewportHovered && ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
			isRightDragging = true;
		} else if (!ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
			isRightDragging = false;
		}
		m_CameraController.SetRightMousePressed(isRightDragging);
		// --------

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

		// has to handle differently when only the viewport size changes,
		// OnWindowResize only called when the entire window size changes
		if (viewportPanelSize.x > 0.0f && viewportPanelSize.y > 0.0f &&
			(viewportPanelSize.x != m_ViewportSize.x || viewportPanelSize.y != m_ViewportSize.y)) {
			m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
			m_CameraController.OnResize(m_ViewportSize.x, m_ViewportSize.y);
		}

		if (m_CurrentViewportTexture.IsValid()) {
			auto* texMgr = RenderCommand::GetTextureManager();
			ImTextureID imguiTexID = (ImTextureID)texMgr->GetGPUHandle(m_CurrentViewportTexture).ptr;
			ImGui::Image(imguiTexID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y });
		}

		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::End();

		//ImGui::ShowDemoWindow(&m_IsImGuiDemoVisible);
	}

	bool EditorLayer::OnWindowResize(WindowResizeEvent& e) {
		RenderCommand::SetViewport(0, 0, e.GetWidth(), e.GetHeight());
		RenderCommand::SetScissors(0, 0, e.GetWidth(), e.GetHeight());
		return false;
	}
}
