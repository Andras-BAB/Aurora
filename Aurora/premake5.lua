project "Aurora"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"
	buildoptions { "/utf-8", "/we4828" }

	targetdir ("%{wks.location}/Binaries/" .. OutputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

	pchheader "aupch.h"
	pchsource "Source/aupch.cpp"

	files { 
		"Source/Aurora/Core/**.h",
		"Source/Aurora/Events/**.h",
		"Source/Aurora/ImGui/**.h",
		"Source/Aurora/Renderer/**.h",
		"Source/Aurora/Scene/**.h",
		"Source/Aurora/Utils/**.h",
		"Source/Aurora/Math/**.h",
		"Source/aupch.h",

		"Source/Aurora/Core/**.cpp",
		"Source/Aurora/Events/**.cpp",
		"Source/Aurora/ImGui/**.cpp",
		"Source/Aurora/Renderer/**.cpp",
		"Source/Aurora/Scene/**.cpp",
		"Source/Aurora/Utils/**.cpp",
		"Source/Aurora/Math/**.cpp",
		"Source/aupch.cpp",

		-- Windows specific
		"Source/Platform/Windows/**.h",
		"Source/Platform/Windows/**.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

	includedirs
	{
		"Source",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.ImGui}",
		--"%{IncludeDir.tinygltf}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.stb}"
	}
	
	links
	{
		"GLFW",
		"ImGui",
		"assimp"
	}

	filter { "options:gfxapi=vulkan" }
		includedirs
		{
			"%{IncludeDir.VulkanSDK}"
		}
		files
		{
			"Source/Platform/Vulkan/**.h",
			"Source/Platform/Vulkan/**.cpp"
		}
		links
		{
			"%{Library.Vulkan}"
		}

	filter { "options:gfxapi=direct3d" }
		files
		{
			"Source/Platform/DirectX/**.h",
			"Source/Platform/DirectX/**.cpp",
		}

	filter "system:windows"
		systemversion "latest"
		defines { }

	filter "configurations:Debug"
		defines { "AU_DEBUG", "_DEBUG" }
		runtime "Debug"
		optimize "Off"
		symbols "On"
		linktimeoptimization "Off"
		
	filter "configurations:Release"
		defines { "AU_RELEASE", "NDEBUG" }
		runtime "Release"
		optimize "Speed"
		symbols "On"
		linktimeoptimization "Off"

	filter "configurations:Dist"
		defines { "AU_DIST", "NDEBUG" }
		runtime "Release"
		optimize "Full"
		symbols "Off"
		staticruntime "On"
		linktimeoptimization "On"