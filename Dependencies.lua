
-- Dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
--IncludeDir["stb_image"] = "%{wks.location}/Core/vendor/stb_image"
--IncludeDir["yaml_cpp"] = "%{wks.location}/Core/vendor/yaml-cpp/include"
--IncludeDir["Box2D"] = "%{wks.location}/Core/vendor/Box2D/include"
IncludeDir["GLFW"] = "%{wks.location}/Aurora/Vendor/GLFW/include"
IncludeDir["glm"] = "%{wks.location}/Aurora/Vendor/glm"
IncludeDir["spdlog"] = "%{wks.location}/Aurora/Vendor/spdlog/include"
IncludeDir["Glad"] = "%{wks.location}/Aurora/Vendor/glad/include"
--IncludeDir["ImGui"] = "%{wks.location}/Core/vendor/ImGui"
--IncludeDir["entt"] = "%{wks.location}/Core/vendor/entt/include"
--IncludeDir["shaderc"] = "%{wks.location}/Aurora/Vendor/shaderc/include"
--IncludeDir["SPIRV_Cross"] = "%{wks.location}/Aurora/Vendor/SPIRV-Cross"
--IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}

--LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["GLFW"] = "%{wks.location}/Aurora/Vendor/GLFW/bin/" .. OutputDir .. "/%{prj.name}"
LibraryDir["Glad"] = "%{wks.location}/Aurora/Vendor/glad/bin/" .. OutputDir .. "/%{prj.name}"

Library = {}

--Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
--Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"
Library["GLFW"] = "%{wks.location}/Core/Vendor/GLFW/bin/" .. OutputDir .. "/%{prj.name}/GLFW.lib"

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
    filter { "options:gfxapi=opengl" }
        include "Aurora/Vendor/glad"
    filter {}
group ""

group "Aurora"
    include "Aurora"
group ""
