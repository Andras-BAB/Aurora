#pragma once

#include "Aurora/Core/PlatformDetection.h"

#include <memory>

#ifdef AU_DEBUG
    #if defined(AU_PLATFORM_WINDOWS)
        #define AU_DEBUGBREAK() __debugbreak()
    #elif defined(AU_PLATFORM_LINUX)
        #include <signal.h>
        #define AU_DEBUGBREAK() raise(SIGTRAP)
    #else
        #error "Platform doesn't support debugbreak yet!"
    #endif
    #define AU_ENABLE_ASSERTS
#else
    #define AU_DEBUGBREAK()
#endif

#define AU_EXPAND_MACRO(x) x
#define AU_STRINGIFY_MACRO(x) #x

#include "Aurora/Core/Log.h"
#include "Aurora/Core/Assert.h"