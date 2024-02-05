#include <aupch.h>

#include <Core/Application.h>

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {

#ifndef DIST

    Aurora::Log::Init();

#endif // DIST

    auto app = Aurora::CreateApplication();
    app->Run();

    delete app;

}