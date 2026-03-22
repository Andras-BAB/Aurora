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
		"{IncludeDir.tinygltf}",
		"%{IncludeDir.assimp}"
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
		defines { "DEBUG" }
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines { "RELEASE" }
		runtime "Release"
		optimize "On"
		symbols "On"

	filter "configurations:Dist"
		defines { "DIST" }
		runtime "Release"
		optimize "On"
		symbols "Off"
		staticruntime "On"
		