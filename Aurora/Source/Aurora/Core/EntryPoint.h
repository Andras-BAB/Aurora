#pragma once
#include "Log.h"
#include "Core/Application.h"

#include <dxgidebug.h>
#include <dxgi1_6.h>
#include <windows.h>

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxgi.lib") 

extern Aurora::Application* Aurora::CreateApplication();

int main(int argc, char** argv) {

	Aurora::Log::Init();

	auto app = Aurora::CreateApplication();
	app->Run();

	delete app;
	
	HRESULT hr = CoInitialize(nullptr);
	IDXGIDebug1* dxgiDebug = nullptr;
	hr = DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug));
	if (SUCCEEDED(hr) && dxgiDebug) {
		dxgiDebug->ReportLiveObjects(
			DXGI_DEBUG_ALL,
			DXGI_DEBUG_RLO_DETAIL
		);
		dxgiDebug->Release();
	}
	CoUninitialize();
}

//int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
//
//	Aurora::Log::Init();
//
//	auto app = Aurora::CreateApplication();
//	app->Run();
//
//	delete app;
//
//	HRESULT hr = CoInitialize(nullptr);
//	IDXGIDebug1* dxgiDebug = nullptr;
//	hr = DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug));
//	if (SUCCEEDED(hr) && dxgiDebug) {
//		dxgiDebug->ReportLiveObjects(
//			DXGI_DEBUG_ALL,
//			DXGI_DEBUG_RLO_DETAIL
//		);
//		dxgiDebug->Release();
//	}
//	CoUninitialize();
//}
