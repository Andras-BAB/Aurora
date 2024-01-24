#include "Core/Core.h"

#ifdef WINDOWS

#include <Windows.h>

#ifndef DIST
#include <iostream>
#endif // !DIST

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {

#ifndef DIST

    AllocConsole();  // Konzol inicializálása
    auto out = freopen("CONOUT$", "w", stdout);  // stdout átirányítása a konzolra
    auto in = freopen("CONIN$", "r", stdin);    // stdin átirányítása a konzolra
    auto err = freopen("CONOUT$", "w", stderr);  // stderr átirányítása a konzolra

#endif // DIST

	Core::PrintHelloWorld();
}

#endif // WINDOWS


#ifdef LINUX

int main()
{
	Core::PrintHelloWorld();
}

#endif // LINUX