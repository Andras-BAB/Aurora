#pragma once

#include "Aurora/Core/Log.h"
#include "Aurora/Core/Application.h"
#include "Aurora/Renderer/RenderCommand.h"
#include "Platform/Vulkan/Renderer/VulkanContext.h"

#include <vulkan/vulkan.h>

namespace vkhelper {

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
    VkBuffer& buffer, VkDeviceMemory& bufferMemory);

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool commandPool);
    
}
