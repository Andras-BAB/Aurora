# App/CMakeLists.txt

include(${CMAKE_SOURCE_DIR}/Dependencies.cmake)

# CMake projekt neve
project(App)

# Beállítások
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Kimeneti és köztes könyvtárak
set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/Binaries/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}")
set(OBJECT_DIR "${CMAKE_SOURCE_DIR}/Binaries/Intermediates/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}")

# Kimeneti és köztes könyvtár beállítása
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})

# Forrásfájlok
file(GLOB_RECURSE SOURCES
    "Source/**/*.cpp"
    "Source/**/*.h"
)

set(SOURCES
    "Source/App.cpp" 
    "Source/SandboxLayer.cpp"
    "Source/SandboxLayer.h"    
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Include könyvtárak
target_include_directories(${PROJECT_NAME} PRIVATE
    "Source"
    "${CMAKE_SOURCE_DIR}/Aurora/Source"
    "${CMAKE_SOURCE_DIR}/Aurora/Source/Aurora"
    "${SPDLOG_INCLUDE_DIR}"
    "${GLM_INCLUDE_DIR}"
    "${GLFW_INCLUDE_DIR}"
    "${GLAD_INCLUDE_DIR}"
    "${ENTT_INCLUDE_DIR}"
    "${TINYGLTF_INCLUDE_DIR}"
)

# Linkelés
target_link_libraries(${PROJECT_NAME} PRIVATE Aurora)

# Shaders folder copy to bin dir
# Forrás mappa és cél mappa
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Shaders")
set(DEST_DIR "${OUTPUT_DIR}/Shaders")

# Custom command a másoláshoz
add_custom_command(
        TARGET App POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${SOURCE_DIR} ${DEST_DIR}
)

# Windows-specifikus beállítások
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS TRUE
    )
    add_definitions(-DWINDOWS)
endif()

# Konfigurációs beállítások
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR})
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DRELEASE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR})
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()
