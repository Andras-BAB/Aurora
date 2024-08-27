#pragma once

#include "Aurora/Core/Base.h"
#include "Aurora/Core/Log.h"
#include <filesystem>

#ifdef AU_ENABLE_ASSERTS

    // Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
    // provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
    #define AU_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { AU##type##ERROR(msg, __VA_ARGS__); AU_DEBUGBREAK(); } }
    #define AU_INTERNAL_ASSERT_WITH_MSG(type, check, ...) AU_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
    #define AU_INTERNAL_ASSERT_NO_MSG(type, check) AU_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", AU_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

    #define AU_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
    #define AU_INTERNAL_ASSERT_GET_MACRO(...) AU_EXPAND_MACRO( AU_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, AU_INTERNAL_ASSERT_WITH_MSG, AU_INTERNAL_ASSERT_NO_MSG) )

    // Currently accepts at least the condition and one additional parameter (the message) being optional
    #define AU_ASSERT(...) AU_EXPAND_MACRO( AU_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
    #define AU_CORE_ASSERT(...) AU_EXPAND_MACRO( AU_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
    #define AU_ASSERT(...)
    #define AU_CORE_ASSERT(...)
#endif