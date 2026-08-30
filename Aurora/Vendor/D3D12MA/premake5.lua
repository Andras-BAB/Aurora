project "D3D12MA"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"
	buildoptions { "/utf-8", "/we4828" }

	targetdir ("%{wks.location}/Binaries/" .. OutputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

	includedirs {
		"%{IncludeDir.D3D12MA}"
	}

	files {
		"src/D3D12MemAlloc.cpp"
	}

	defines {
		--"D3D12MA_D3D12_HEADERS_ALREADY_INCLUDED",
		--"D3D12MA_USE_SMALL_RESOURCE_PLACEMENT_ALIGNMENT"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines {
			"D3D12MA_DEBUG_MARGIN"
		}
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

