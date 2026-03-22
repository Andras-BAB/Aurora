#pragma once

#include "LayerStack.h"
#include "Aurora/Core/Window.h"
#include "Aurora/Events/ApplicationEvent.h"
#include "Aurora/ImGui/ImGuiLayer.h"
#include "Aurora/Scene/AssetRegistry.h"

namespace Aurora {

	class Application {
	public:
		Application();
		virtual ~Application();

		void Run();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* layer);

		void OnEvent(Event& e);
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		void ImGuiBlockEvents(bool block) const;

		AssetRegistry& GetAssetRegistry() { return m_AssetRegistry; }
		
		Window& GetWindow() const { return *m_Window; }
		static Application& Get() { return *s_Instance; }

		void Close();
		
	private:
		std::unique_ptr<Window> m_Window;

		LayerStack m_LayerStack;
		ImGuiLayer* m_ImGuiLayer;

		AssetRegistry m_AssetRegistry;

		double m_LastFrameTime = 0.0f;
		bool m_Minimized;

		bool m_Running;

		// std::unique_ptr<IGraphicsInstance> m_GraphicsInstance;
	private:
		static Application* s_Instance;
		//friend int ::main(int argc, char** argv);
	};

	Application* CreateApplication();

}
