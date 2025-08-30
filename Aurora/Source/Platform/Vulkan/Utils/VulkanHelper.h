#pragma once

#include "Platform/Vulkan/Renderer/VulkanBuffer.h"
#include "Aurora/Core/Assert.h"

#include <vulkan/vulkan.h>

#ifdef AU_DEBUG
    #define VK_CHECK(f) { \
        VkResult res = (f); \
        if(res != VK_SUCCESS) { \
            AU_CORE_ERROR("Vulkan Error: {0} at {1}:{2}", vkhelper::VkResultToString(res), __FILE__, __LINE__); AU_DEBUGBREAK(); \
        } \
    }
#else
    #define VK_CHECK(f) { \
        VkResult res = (f); \
        if(res != VK_SUCCESS) { \
            AU_CORE_ERROR("Vulkan Error: {0} at {1}:{2}", vkhelper::VkResultToString(res), __FILE__, __LINE__); \
        } \
    }
#endif

namespace vkhelper {

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
    VkBuffer& buffer, VkDeviceMemory& bufferMemory);

    Aurora::VulkanBuffer createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
        const void* data = nullptr, VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE);

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    inline const char* VkResultToString(VkResult result) {
        switch (result) {
        case VK_SUCCESS: return "VK_SUCCESS";
        case VK_NOT_READY: return "VK_NOT_READY";
        case VK_TIMEOUT: return "VK_TIMEOUT";
        case VK_EVENT_SET: return "VK_EVENT_SET";
        case VK_EVENT_RESET: return "VK_EVENT_RESET";
        case VK_INCOMPLETE: return "VK_INCOMPLETE";
        case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
        case VK_ERROR_MEMORY_MAP_FAILED: return "VK_ERROR_MEMORY_MAP_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT: return "VK_ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT: return "VK_ERROR_FEATURE_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER: return "VK_ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_TOO_MANY_OBJECTS: return "VK_ERROR_TOO_MANY_OBJECTS";
        case VK_ERROR_FORMAT_NOT_SUPPORTED: return "VK_ERROR_FORMAT_NOT_SUPPORTED";
        case VK_ERROR_FRAGMENTED_POOL: return "VK_ERROR_FRAGMENTED_POOL";
        default: return "Unknown VkResult";
        }
    }

}
