#include <aupch.h>

#include <Events/Event.h>
#include <Core/Application.h>
#include <Core/Log.h>

namespace Aurora {

	Application* Aurora::Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpecs specs) {
	
		if (s_Instance != nullptr) {
			AuCoreLogError("Application already exists!");
		} else {
			s_Instance = this;
		}

		m_Window = Window::Create(WindowProps());
		//m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
		m_Window->SetEventCallback(BindEvent(Application::OnEvent));

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application() {

	}

	void Application::OnEvent(Event& e) {

		EventDispatcher dispatcher(e);

		//dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));
		dispatcher.Dispatch<WindowCloseEvent>(BindEvent(Application::OnWindowClose));

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
			if (e.Handled)
				break;
			(*it)->OnEvent(e);
		}

	}

	void Application::Close() {
		m_Running = false;
	}

	bool Application::OnWindowClose(WindowCloseEvent& e) {
		m_Running = false;
		return true;
	}

	void Application::PushLayer(Layer* layer) {
		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* layer) {
		m_LayerStack.PushOverlay(layer);
	}

	void Application::Run() {

		while (m_Running) {
			if (const auto ecode = Window::ProcessMessages()) {
				m_Running = false;
				AuCoreLogCritical("NEEDS FIX! WHEN CLOSING THE WINDOW IT THROWS AN EXCEPTION FROM LAYERSTACK DESTROYER!");
			}
			for (Layer* layer : m_LayerStack) {
				layer->OnUpdate();
			}

			m_ImGuiLayer->Begin();
			{
				for (Layer* layer : m_LayerStack)
					layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();
		
		}

	}

}

