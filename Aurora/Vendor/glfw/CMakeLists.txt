include(${CMAKE_SOURCE_DIR}/Dependencies.cmake)

project(GLFW LANGUAGES C)

set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/Binaries/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}")
set(OBJECT_DIR "${CMAKE_SOURCE_DIR}/Binaries/Intermediates/${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})

set(SOURCES
        "include/GLFW/glfw3.h"
        "include/GLFW/glfw3native.h"
        #"src/glfw_config.h"
        "src/context.c"
        "src/init.c"
        "src/input.c"
        "src/monitor.c"

        "src/null_init.c"
        "src/null_joystick.c"
        "src/null_monitor.c"
        "src/null_window.c"

        "src/platform.c"
        "src/vulkan.c"
        "src/window.c"
        "src/win32_init.c"
        "src/win32_joystick.c"
        "src/win32_module.c"
        "src/win32_monitor.c"
        "src/win32_time.c"
        "src/win32_thread.c"
        "src/win32_window.c"
        "src/wgl_context.c"
        "src/egl_context.c"
        "src/osmesa_context.c"
)

add_definitions(-D_GLFW_WIN32 -D_CRT_SECURE_NO_WARNINGS)

add_library("${PROJECT_NAME}" STATIC ${SOURCES})