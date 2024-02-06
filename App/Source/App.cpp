#include <Aurora.h>

namespace Aurora {

    class TestLayer : public Layer {
    public:

        TestLayer() : Layer("TestLayer") { }

        void OnEvent(Event& e) override {
            EventDispatcher dispatcher(e);
            dispatcher.Dispatch<AppUpdateEvent>(BindEvent(TestLayer::OnTestEvent));
        }

        bool OnTestEvent(AppUpdateEvent& e) {
            //AuLogInfo("TestEvent success!");
            return true;
        }

    };

    class AuApp : public Application {
    public:

        AuApp(const ApplicationSpecs& specs) : Application(specs) {
            PushLayer(new TestLayer());
        }

    };

    Application* CreateApplication() {
        ApplicationSpecs specs{};
        specs.Name = "AuroraApp";

        return new AuApp(specs);
    }

}
