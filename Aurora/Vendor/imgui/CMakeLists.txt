include(${CMAKE_SOURCE_DIR}/Dependencies.cmake)

project(ImGui)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/Binaries/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}")
set(OBJECT_DIR "${CMAKE_SOURCE_DIR}/Binaries/Intermediates/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})

set(SOURCES 
    "imconfig.h"
    "imgui.cpp"
    "imgui.h"
    "imgui_demo.cpp"
    "imgui_draw.cpp"
    "imgui_internal.h"
    "imgui_tables.cpp"
    "imgui_widgets.cpp"
    "imstb_rectpack.h"
    "imstb_textedit.h"
    "imstb_truetype.h"
    "backends/imgui_impl_vulkan.cpp"
    "backends/imgui_impl_glfw.cpp"
)

add_library("${PROJECT_NAME}" STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${IMGUI_INCLUDE_DIR} ${VULKAN_INCLUDE_DIR} ${GLFW_INCLUDE_DIR})
