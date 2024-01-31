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
#include <spdlog/sinks/stdout_color_sinks.h>
int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {

#ifndef DIST
    //Aurora::Log::Init();
    AllocConsole();  // Konzol inicializálása
    auto out = freopen("CONOUT$", "w", stdout);  // stdout átirányítása a konzolra
    auto in = freopen("CONIN$", "r", stdin);    // stdin átirányítása a konzolra
    auto err = freopen("CONOUT$", "w", stderr);  // stderr átirányítása a konzolra
    
    std::shared_ptr<spdlog::logger> logger = spdlog::stdout_color_mt("console");

    Aurora::Log::SetLogger(logger);

	Core::PrintHelloWorld();


    
#endif // DIST

    auto app = Aurora::CreateApplication();
    app->Run();


    delete app;

}
