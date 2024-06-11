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
        "Vendor/glfw/include",
        "%{IncludeDir.glm}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.VulkanSDK}"
    }

    --[[
    libdirs
    {
        "%{LibraryDir.VulkanSDK}"
    }
    ]]--

    filter { "options:gfxapi=vulkan" }
        links
        {
            "GLFW",
            "%{Library.Vulkan}"
        }
    filter { "options:gfxapi=opengl" }
        links
        {
            "GLFW",
            "Glad"
        }

    filter "system:windows"
        systemversion "latest"
        defines { }

    filter "configurations:Debug"
        defines { "DEBUG" }
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines { "RELEASE" }
        runtime "Release"
        optimize "On"
        symbols "On"
