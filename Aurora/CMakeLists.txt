# Aurora/CMakeLists.txt

include(${CMAKE_SOURCE_DIR}/Dependencies.cmake)

# CMake projekt név
project(Aurora)

# Beállítások
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Kimeneti és köztes könyvtárak
set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/Binaries/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}")
set(OBJECT_DIR "${CMAKE_SOURCE_DIR}/Binaries/Intermediates/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}")

# Kimeneti és köztes könyvtár beállítása
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})

# Forrásfájlok
file(GLOB_RECURSE SOURCES
    "Source/**/*.cpp"
    "Source/**/*.h"
)

# Statikus könyvtár létrehozása
add_library(${PROJECT_NAME} STATIC ${SOURCES})

# PCH beállítások
target_precompile_headers(${PROJECT_NAME} PRIVATE "Source/aupch.cpp")

# Include könyvtárak
target_include_directories(${PROJECT_NAME} PRIVATE
    "Source"
    "${GLFW_INCLUDE_DIR}"
    "${GLM_INCLUDE_DIR}"
    "${SPDLOG_INCLUDE_DIR}"
    "${GLAD_INCLUDE_DIR}"
    "${IMGUI_INCLUDE_DIR}"
    "${ENTT_INCLUDE_DIR}"
    "${TINYGLTF_INCLUDE_DIR}" 
    #"${FILEWATCH_INCLUDE_DIR}"
)

add_subdirectory(Vendor/glad)
add_subdirectory(Vendor/glfw)
add_subdirectory(Vendor/imgui)

# Linkelés
target_link_libraries(${PROJECT_NAME} PRIVATE Glad GLFW ImGui)

# Vulkan és OpenGL beállítások
if(DEFINED GFXAPI_VULKAN)
    target_include_directories(${PROJECT_NAME} PRIVATE "${IncludeDir_VulkanSDK}")
    target_link_libraries(${PROJECT_NAME} PRIVATE "${Library_Vulkan}")
elseif(DEFINED GFXAPI_OPENGL)
    target_include_directories(${PROJECT_NAME} PRIVATE "${IncludeDir_Glad}")
    target_link_libraries(${PROJECT_NAME} PRIVATE Glad)
endif()

# Windows-specifikus beállítások
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS TRUE
    )
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-DGLFW_INCLUDE_NONE)
endif()

# Konfigurációs beállítások
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DAU_DEBUG)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR})
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O3")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DAU_RELEASE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR})
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()
