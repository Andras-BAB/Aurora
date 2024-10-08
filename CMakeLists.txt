cmake_minimum_required(VERSION 3.29)

project(Aurora)

#set(CMAKE_GENERATOR_PLATFORM x64)

# Defining configurations
set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE STRING "")

# Setting the output directory
set(OUTPUT_DIR "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}/${CMAKE_BUILD_TYPE}")

# MSVC specific options
if(MSVC)
    add_compile_options("/EHsc" "/Zc:preprocessor" "/Zc:__cplusplus")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()


include(Dependencies.cmake)
add_subdirectory(Aurora)
add_subdirectory(App)