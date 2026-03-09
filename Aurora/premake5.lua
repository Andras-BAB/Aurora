project "Aurora"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir ("%{wks.location}/Binaries/" .. OutputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

    pchheader "aupch.h"
    pchsource "Source/aupch.cpp"

    files { "Source/**.h", "Source/**.cpp" }

    defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

    includedirs
    {
        "Source",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.tinygltf}"
    }
    links
    {
        "GLFW",
        "ImGui"
    }

    --[[
    libdirs
    {
        "%{LibraryDir.VulkanSDK}"
    }
    ]]--

    filter { "options:gfxapi=vulkan" }
        includedirs
        {
            "%{IncludeDir.VulkanSDK}"
        }
        links
        {
            "%{Library.Vulkan}"
        }
        
    filter { "options:gfxapi=opengl" }
        includedirs
        {
            "%{IncludeDir.Glad}"
        }
        links
        {
            "Glad"
        }

    filter "system:windows"
        systemversion "latest"
        defines { }

    filter "configurations:Debug"
        defines { "AU_DEBUG" }
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines { "AU_RELEASE" }
        runtime "Release"
        optimize "On"
        symbols "On"
