#include "Core/EntryPoint.h"

#include "SandboxLayer.h"

namespace Aurora {

	class App : public Application {
	public:
		App() {
			PushLayer(new Sandbox::SandboxLayer());
		}
	};

	Application* CreateApplication() {
		return new App();
	}

}
