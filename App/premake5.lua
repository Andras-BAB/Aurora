project "App"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

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

        -- Include Core dependencies
        "%{IncludeDir.VulkanSDK}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.GLFW}"
    }

    links
    {
        "Aurora"
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
