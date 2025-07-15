# Dependencies

set(VULKAN_SDK_DIR $ENV{VULKAN_SDK})
if(NOT VULKAN_SDK_DIR)
    message(FATAL_ERROR "Vulkan SDK not found!")
endif()

set(VULKAN_INCLUDE_DIR "${VULKAN_SDK_DIR}/Include")
set(VULKAN_LIBRARY_DIR "${VULKAN_SDK_DIR}/Lib")

if (WIN32)
    set(VULKAN_LIB_NAME "vulkan-1.lib")
elseif (UNIX)
    set(VULKAN_LIB_NAME "vulkan")
else ()
    message(FATAL_ERROR "Unsupported platform")
endif ()

set(GLFW_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Aurora/Vendor/GLFW/include" CACHE STRING "GLFW include directory")
set(GLM_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Aurora/Vendor/glm" CACHE STRING "GLM include directory")
set(SPDLOG_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Aurora/Vendor/spdlog/include" CACHE STRING "spdlog include directory")
set(GLAD_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Aurora/Vendor/glad/include" CACHE STRING "Glad include directory")
set(IMGUI_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Aurora/Vendor/ImGui" CACHE STRING "ImGui include directory")
set(ENTT_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Aurora/Vendor/entt/include" CACHE STRING "entt include directory")
set(TINYGLTF_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Aurora/Vendor/tinygltf/include" CACHE STRING "tinygltf include directory")
#set(FILEWATCH_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Aurora/Vendor/filewatch" CACHE STRING "FileWatch include directory")
