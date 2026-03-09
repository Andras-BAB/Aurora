#pragma once
#include "Log.h"
#include "Core/Application.h"

extern Aurora::Application* Aurora::CreateApplication();

int main(int argc, char** argv) {

	Aurora::Log::Init();

	auto app = Aurora::CreateApplication();
	app->Run();

	delete app;
}