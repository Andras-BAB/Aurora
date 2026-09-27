#include "Core/EntryPoint.h"

#include "EditorLayer.h"

namespace Aurora {

	class App : public Application {
	public:
		App() {
			PushLayer(new EditorLayer());
		}
	};

	Application* CreateApplication() {
		return new App();
	}

}
