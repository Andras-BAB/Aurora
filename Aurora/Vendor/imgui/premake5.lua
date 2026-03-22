project "ImGui"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
    staticruntime "off"

	targetdir ("%{wks.location}/Binaries/" .. OutputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

    includedirs
    {
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.ImGui}"
    }

	files
	{
		"imconfig.h",
		"imgui.h",
		"imgui.cpp",
		"imgui_draw.cpp",
		"imgui_internal.h",
		"imgui_widgets.cpp",
		"imstb_rectpack.h",
		"imstb_textedit.h",
		"imstb_truetype.h",
        "imgui_tables.cpp",
		"imgui_demo.cpp",
		
        -- Platform related
        "backends/imgui_impl_dx12.h",
        "backends/imgui_impl_dx12.cpp",
        "backends/imgui_impl_glfw.h",
        "backends/imgui_impl_glfw.cpp"
	}

	filter "system:windows"
		systemversion "latest"

	filter "system:linux"
		pic "On"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
			
	filter "configurations:Dist"
		defines { "DIST" }
		runtime "Release"
		optimize "On"
		symbols "Off"
		staticruntime "On"
