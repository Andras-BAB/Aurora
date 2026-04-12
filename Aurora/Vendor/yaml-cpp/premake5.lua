project "yaml-cpp"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"
	buildoptions { "/utf-8", "/we4828" }
	warnings "off"
	
	targetdir ("%{wks.location}/Binaries/" .. OutputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")
	
	files
	{
		"src/**.h",
		"src/**.cpp",
		
		"include/**.h"
	}
	
	includedirs
	{
		"include"
	}
	
	defines
	{
		"YAML_CPP_STATIC_DEFINE"
	}
	
	filter "system:windows"
		systemversion "latest"
		defines { }

	filter "configurations:Debug"
		runtime "Debug"
		optimize "Off"
		symbols "On"
		linktimeoptimization "Off"
		
	filter "configurations:Release"
		runtime "Release"
		optimize "Speed"
		symbols "On"
		linktimeoptimization "Off"

	filter "configurations:Dist"
		runtime "Release"
		optimize "Full"
		symbols "Off"
		staticruntime "On"
		linktimeoptimization "On"
	