#pragma once

#include <string>

#include <Events/Event.h>
#include <Events/ApplicationEvent.h>
#include <Core/Base.h>

#include <Core/Window.h>

#include <Core/LayerStack.h>
#include <ImGui/ImGuiLayer.h>

namespace Aurora {

	struct ApplicationSpecs {
		std::string Name = "Aurora";
	};

	class Application {
	public:

		Application(const ApplicationSpecs specs);
		virtual ~Application();

		void OnEvent(Event& e);
		void Close();
		bool OnWindowClose(WindowCloseEvent& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		void Run();

	private:

		bool m_Running = true;

		std::unique_ptr<Window> m_Window;

		ImGuiLayer* m_ImGuiLayer;
		LayerStack m_LayerStack;

	private:

		static Application* s_Instance;

	};

	// Client has to define it
	Application* CreateApplication();

}