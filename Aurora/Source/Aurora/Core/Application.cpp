#include "aupch.h"
#include "Aurora/Core/Application.h"

#include "Aurora/Events/ApplicationEvent.h"
#include "Aurora/Renderer/Renderer.h"
#include "Aurora/Utils/PlatformUtils.h"

namespace Aurora {

	Application* Application::s_Instance = nullptr;

	Application::Application() {
		s_Instance = this;
		m_Window = Window::Create(WindowProps());
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
		m_Running = true;
		m_Minimized = false;

		Renderer::Init();
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	void Application::Run() {

		while (m_Running) {

			float time = Time::GetTime();
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized) {
				{
					for (Layer* layer : m_LayerStack)
						layer->OnUpdate(timestep);

				}

				m_ImGuiLayer->Begin();
				{
					for (Layer* layer : m_LayerStack)
						layer->OnImGuiRender();
				}
				m_ImGuiLayer->End();
			}

			m_Window->OnUpdate();
		}
	}

	void Application::PushLayer(Layer* layer) {
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer) {
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::OnEvent(Event& e) {

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
			if (e.Handled)
				break;
			(*it)->OnEvent(e);
		}

	}

	bool Application::OnWindowClose(WindowCloseEvent& e) {
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e) {
		if (e.GetWidth() == 0 || e.GetHeight() == 0) {
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}

	void Application::Close() {
		m_Running = false;
	}
}
