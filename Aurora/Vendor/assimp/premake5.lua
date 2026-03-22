project "assimp"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"
	warnings "off"

	targetdir ("%{wks.location}/Binaries/" .. OutputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

	includedirs {
		".",
		"include",
		--"include/assimp",
		"contrib",
		"contrib/rapidjson/include",
		"contrib/zlib",
		"contrib/unzip",
		"contrib/utf8cpp/source",
		"contrib/pugixml/src",
		--"contrib/earcut-hpp",
		"code"
	}

	files { 
		-- Core
		"code/Common/**.cpp",
		"code/Common/**.h",
		"code/PostProcessing/**.cpp",
		"code/PostProcessing/**.h",
		"code/Material/**.cpp",
		"code/Material/**.h",
		"code/Geometry/**.cpp",
		"code/Geometry/**.h",
		"code/CApi/**.cpp",
		"code/CApi/**.h",
		"code/SceneCombiner/**.cpp",
		"code/SceneCombiner/**.h",

		-- glTF
		"code/AssetLib/glTF/**.cpp",
		"code/AssetLib/glTF/**.h",
		"code/AssetLib/glTF2/**.cpp",
		"code/AssetLib/glTF2/**.h",
		"code/AssetLib/glTFCommon/**.cpp",
		"code/AssetLib/glTFCommon/**.h",

		-- FBX
		"code/AssetLib/FBX/**.cpp",
		"code/AssetLib/FBX/**.h",

		-- OBJ
		"code/AssetLib/Obj/**.cpp",
		"code/AssetLib/Obj/**.h",

		"contrib/pugixml/src/*.cpp",

		-- zlib / unzip
		"contrib/zlib/*.c",
		"contrib/zlib/*.h",
		"contrib/unzip/*.c",
		"contrib/unzip/*.h",

		-- Headers
		"include/**.h"
	}

	defines { 
		"_CRT_SECURE_NO_WARNINGS",
		"ASSIMP_BUILD_NO_EXPORT",
		--"ASSIMP_BUILD_NO_OWN_ZLIB",

		"ASSIMP_BUILD_NO_3DS_IMPORTER",
		"ASSIMP_BUILD_NO_3D_IMPORTER",
		"ASSIMP_BUILD_NO_AC_IMPORTER",
		"ASSIMP_BUILD_NO_AMF_IMPORTER",
		"ASSIMP_BUILD_NO_ASE_IMPORTER",
		"ASSIMP_BUILD_NO_ASSBIN_IMPORTER",
		"ASSIMP_BUILD_NO_ASSXML_IMPORTER",
		"ASSIMP_BUILD_NO_B3D_IMPORTER",
		"ASSIMP_BUILD_NO_BLEND_IMPORTER",
		"ASSIMP_BUILD_NO_BVH_IMPORTER",
		"ASSIMP_BUILD_NO_COB_IMPORTER",
		"ASSIMP_BUILD_NO_COLLADA_IMPORTER",
		"ASSIMP_BUILD_NO_CSM_IMPORTER",
		"ASSIMP_BUILD_NO_DXF_IMPORTER",
		"ASSIMP_BUILD_NO_HMP_IMPORTER",
		"ASSIMP_BUILD_NO_IFC_IMPORTER",
		"ASSIMP_BUILD_NO_IRR_IMPORTER",
		"ASSIMP_BUILD_NO_IRRMESH_IMPORTER",
		"ASSIMP_BUILD_NO_LWO_IMPORTER",
		"ASSIMP_BUILD_NO_LWS_IMPORTER",
		"ASSIMP_BUILD_NO_MD2_IMPORTER",
		"ASSIMP_BUILD_NO_MD3_IMPORTER",
		"ASSIMP_BUILD_NO_MD5_IMPORTER",
		"ASSIMP_BUILD_NO_MDC_IMPORTER",
		"ASSIMP_BUILD_NO_MDL_IMPORTER",
		"ASSIMP_BUILD_NO_MS3D_IMPORTER",
		"ASSIMP_BUILD_NO_NDO_IMPORTER",
		"ASSIMP_BUILD_NO_NFF_IMPORTER",
		"ASSIMP_BUILD_NO_OFF_IMPORTER",
		"ASSIMP_BUILD_NO_OGRE_IMPORTER",
		"ASSIMP_BUILD_NO_OPENGEX_IMPORTER",
		"ASSIMP_BUILD_NO_PLY_IMPORTER",
		"ASSIMP_BUILD_NO_Q3BSP_IMPORTER",
		"ASSIMP_BUILD_NO_Q3D_IMPORTER",
		"ASSIMP_BUILD_NO_RAW_IMPORTER",
		"ASSIMP_BUILD_NO_SIB_IMPORTER",
		"ASSIMP_BUILD_NO_SMD_IMPORTER",
		"ASSIMP_BUILD_NO_STL_IMPORTER",
		"ASSIMP_BUILD_NO_TERRAGEN_IMPORTER",
		"ASSIMP_BUILD_NO_X_IMPORTER",
		"ASSIMP_BUILD_NO_XGL_IMPORTER",
		"ASSIMP_BUILD_NO_USD_IMPORTER",
		"ASSIMP_BUILD_NO_C4D_IMPORTER",
		"ASSIMP_BUILD_NO_3MF_IMPORTER",
		"ASSIMP_BUILD_NO_X3D_IMPORTER",
		"ASSIMP_BUILD_NO_MMD_IMPORTER",
		"ASSIMP_BUILD_NO_M3D_IMPORTER",
		"ASSIMP_BUILD_NO_IQM_IMPORTER"
	}

	filter "system:windows"
		systemversion "latest"
		defines { "_CRT_SECURE_NO_WARNINGS" }

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
