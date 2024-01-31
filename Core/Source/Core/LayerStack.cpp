#include <aupch.h>

#include <Core/LayerStack.h>

Aurora::LayerStack::~LayerStack() {
	for (Layer* layer : m_Stack) {
		layer->OnDestroy();
		delete layer;
	}
}

void Aurora::LayerStack::PushLayer(Layer* layer) {

	m_Stack.emplace(m_Stack.begin() + m_InsertIndex, layer);
	layer->OnCreate();
	m_InsertIndex++;
}

void Aurora::LayerStack::PushOverlay(Layer* overlay) {
	m_Stack.emplace_back(overlay);
	overlay->OnCreate();
}

void Aurora::LayerStack::PopLayer(Layer* layer) {
	auto it = std::find(m_Stack.begin(), m_Stack.begin() + m_InsertIndex, layer);
	if (it != m_Stack.begin() + m_InsertIndex) {
		layer->OnDestroy();
		m_Stack.erase(it);
		m_InsertIndex--;
	}
}

void Aurora::LayerStack::PopOverlay(Layer* overlay) {
	auto it = std::find(m_Stack.begin() + m_InsertIndex, m_Stack.end(), overlay);
	if (it != m_Stack.end()) {
		overlay->OnDestroy();
		m_Stack.erase(it);
	}
}
