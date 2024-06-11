#pragma once

#include "LayerStack.h"
#include "Core/Window.h"
#include "Event/ApplicationEvent.h"

namespace Aurora {

	class Application {
	public:
		Application();
		virtual ~Application() = default;

		void Run();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		void OnEvent(Event& e);
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		Window& GetWindow() { return *m_Window; }
		static Application& Get() { return *s_Instance; }

		void Close();


	private:

		LayerStack m_LayerStack;

		float m_LastFrameTime = 0.0f;
		bool m_Minimized;

		std::unique_ptr<Window> m_Window;
		bool m_Running;

	private:
		static Application* s_Instance;
		//friend int ::main(int argc, char** argv);
	};

	Application* CreateApplication();

}
