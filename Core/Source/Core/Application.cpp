#include <aupch.h>

#include <Events/Event.h>
#include <Core/Application.h>
#include <Core/Log.h>

Aurora::Application* Aurora::Application::s_Instance = nullptr;

Aurora::Application::Application(const ApplicationSpecs specs) {
	Aurora::Log::LogCore();
	if (s_Instance != nullptr) {
		// ERROR, APP ALREADY EXISTS
	} else {
		s_Instance = this;
	}

	m_Window = Window::Create(WindowProps());
	//m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
	m_Window->SetEventCallback(BindEvent(Application::OnEvent));

	m_ImGuiLayer = new ImGuiLayer();
	PushOverlay(m_ImGuiLayer);
}

Aurora::Application::~Application() {

	delete m_ImGuiLayer;
}

void Aurora::Application::OnEvent(Event& e) {

	EventDispatcher dispatcher(e);

	//dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));
	dispatcher.Dispatch<WindowCloseEvent>(BindEvent(Application::OnWindowClose));

	for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
		if (e.Handled)
			break;
		(*it)->OnEvent(e);
	}

}

void Aurora::Application::Close() {
	m_Running = false;
}

bool Aurora::Application::OnWindowClose(WindowCloseEvent& e) {
	m_Running = false;
	return true;
}

void Aurora::Application::PushLayer(Layer* layer) {
	m_LayerStack.PushLayer(layer);
}

void Aurora::Application::PushOverlay(Layer* layer) {
	m_LayerStack.PushOverlay(layer);
}

void Aurora::Application::Run() {

	while (m_Running) {
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


