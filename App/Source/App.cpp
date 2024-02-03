#include <Aurora.h>

namespace Aurora {

    class AuApp : public Application {
    public:

        AuApp(const ApplicationSpecs& specs) : Application(specs) {
            
        }

    };

    Application* CreateApplication() {
        ApplicationSpecs specs{};
        specs.Name = "AuroraApp";

        return new AuApp(specs);
    }

}

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {

#ifndef DIST

    Aurora::Log::Init();

#endif // DIST

    auto app = Aurora::CreateApplication();
    app->Run();


    delete app;

}
