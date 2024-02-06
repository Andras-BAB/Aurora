#pragma once

#include <Events/Event.h>

namespace Aurora {

	class Layer {

	public:
		Layer(const std::string& name = "Layer") : m_DebugName(name) {}
		virtual ~Layer() = default;

		virtual void OnCreate() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate() {}
		virtual void OnEvent(Event& e) {}
		virtual void OnImGuiRender() {}

	#ifdef DEBUG
		std::string GetDebugName() const { return m_DebugName; }
	#endif // DEBUG

	private:

		std::string m_DebugName;

	};

}