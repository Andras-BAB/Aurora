#pragma once

#include <vector>
#include <Core/Layer.h>

namespace Aurora {

	class LayerStack {

	public:
		LayerStack() = default;
		virtual ~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* layer);

		std::vector<Layer*>::iterator begin() { return m_Stack.begin(); }
		std::vector<Layer*>::iterator end() { return m_Stack.end(); }
		std::vector<Layer*>::reverse_iterator rbegin() { return m_Stack.rbegin(); }
		std::vector<Layer*>::reverse_iterator rend() { return m_Stack.rend(); }
		
		std::vector<Layer*>::const_iterator begin() const { return m_Stack.begin(); }
		std::vector<Layer*>::const_iterator end() const { return m_Stack.end(); }
		std::vector<Layer*>::const_reverse_iterator rbegin() const { return m_Stack.rbegin(); }
		std::vector<Layer*>::const_reverse_iterator rend() const { return m_Stack.rend(); }

	private:

		std::vector<Layer*> m_Stack;
		unsigned int m_InsertIndex = 0;

	};

}