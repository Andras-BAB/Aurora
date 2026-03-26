project "App"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"
	buildoptions { "/utf-8", "/we4828" }

	--targetdir "Binaries/%{cfg.buildcfg}"
	--objdir "Binaries-Int/%{cfg.buildcfg}"
	targetdir ("../Binaries/" .. OutputDir .. "/%{prj.name}")
	objdir ("../Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

	files { "Source/**.h", "Source/**.cpp" }

	includedirs
	{
		"Source",

		-- Include Core
		"../Aurora/Source",
		"../Aurora/Source/Aurora",

		-- Include Core dependencies
		"%{IncludeDir.spdlog}",
		--"%{IncludeDir.glm}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.entt}",
		--"{IncludeDir.tinygltf}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.stb}"
	}

	links
	{
		"Aurora"
	}

	filter { "options:gfxapi=vulkan" }
		includedirs
		{
			"%{IncludeDir.VulkanSDK}"
		}

	filter "system:windows"
		systemversion "latest"
		defines { "WINDOWS" }

	filter "configurations:Debug"
		defines { "DEBUG", "_DEBUG" }
		runtime "Debug"
		symbols "On"
		optimize "Off"
		linktimeoptimization "Off"

	filter "configurations:Release"
		defines { "RELEASE", "NDEBUG" }
		runtime "Release"
		optimize "Speed"
		symbols "On"
		linktimeoptimization "Off"

	filter "configurations:Dist"
		defines { "DIST", "NDEBUG" }
		runtime "Release"
		optimize "Full"
		symbols "Off"
		staticruntime "On"
		linktimeoptimization "On"
		