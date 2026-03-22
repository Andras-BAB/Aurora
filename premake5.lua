include "RemoveBOM.lua"

workspace "Aurora"
	architecture "x64"
	configurations { "Debug", "Release", "Dist" }
	startproject "App"
	remove_bom "On"

	filter "system:windows"
		buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus", "/utf-8", "/we4828" }
		multiprocessorcompile "On"
	filter {}

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

newoption {
	trigger = "gfxapi",
	value = "API",
	description = "Choose a particular 3D API for rendering",
	allowed = {
		{ "vulkan", "Vulkan" },
		{ "direct3d", "Direct3D (Windows only)" }
	},
	default = "direct3d"
}

include "Dependencies.lua"

include "App/premake5.lua"