project "ImGui"
	kind "StaticLib"
	language "C++"

	targetdir ("bin/" .. OutputDir .. "/%{prj.name}")
	objdir ("bin-int/" .. OutputDir .. "/%{prj.name}")

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
        "backends/imgui_impl_opengl3.h",
        "backends/imgui_impl_opengl3.cpp",
        "backends/imgui_impl_glfw.h",
        "backends/imgui_impl_glfw.cpp"
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++20"
		staticruntime "On"

	filter "system:linux"
		pic "On"
		systemversion "latest"
		cppdialect "C++20"
		staticruntime "On"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
