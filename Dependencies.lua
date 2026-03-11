
-- Dependencies

--VULKAN_SDK = os.getenv("VULKAN_SDK")

VendorDir = "%{wks.location}/Aurora/Vendor"

IncludeDir = {}
--IncludeDir["stb_image"] = "%{VendorDir}/vendor/stb_image"
--IncludeDir["yaml_cpp"] = "%{VendorDir}/vendor/yaml-cpp/include"
--IncludeDir["Box2D"] = "%{VendorDir}/vendor/Box2D/include"
IncludeDir["GLFW"] = "%{VendorDir}/GLFW/include"
--IncludeDir["glm"] = "%{VendorDir}/Vendor/glm"
IncludeDir["spdlog"] = "%{VendorDir}/spdlog/include"
IncludeDir["ImGui"] = "%{VendorDir}/imgui"
IncludeDir["entt"] = "%{VendorDir}/entt/include"
IncludeDir["tinygltf"] = "%{VendorDir}/tinygltf/include"
IncludeDir["assimp"] = "%{VendorDir}/assimp/include"
--IncludeDir["shaderc"] = "%{wks.location}/Aurora/Vendor/shaderc/include"
--IncludeDir["SPIRV_Cross"] = "%{wks.location}/Aurora/Vendor/SPIRV-Cross"
--IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}

--LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["GLFW"] = "%{VendorDir}/GLFW/bin/" .. OutputDir .. "/%{prj.name}"

Library = {}

--Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
--Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"
--Library["GLFW"] = "%{wks.location}/Aurora/Vendor/GLFW/bin/" .. OutputDir .. "/%{prj.name}/GLFW.lib"

--Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
--Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
--Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"
--Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK}/SPIRV-Toolsd.lib"

--Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
--Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
--Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"

-- Windows
--Library["WinSock"] = "Ws2_32.lib"
--Library["WinMM"] = "Winmm.lib"
--Library["WinVersion"] = "Version.lib"
--Library["BCrypt"] = "Bcrypt.lib"

group "Dependencies"
	include "Aurora/Vendor/glfw"
	include "Aurora/Vendor/imgui"
	include "Aurora/Vendor/assimp"
group ""

group "Aurora"
	include "Aurora"
group ""
