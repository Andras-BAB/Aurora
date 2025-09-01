#include "aupch.h"

#include "VulkanHelper.h"

#include "Aurora/Core/Log.h"
#include "Aurora/Core/Application.h"
#include "Platform/Vulkan/Renderer/VulkanContext.h"
#include "Aurora/Renderer/RenderCommand.h"

#include "Aurora/Core/Assert.h"
#include "Platform/Vulkan/Renderer/VulkanRenderCommand.h"

namespace vkhelper {
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDevice physicalDevice = Aurora::VulkanRenderCommand::GetPhysicalDevice();
		
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
	
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		AU_CORE_CRITICAL("Failed to find suitable memory type!");
		return 0;
	}

	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
	VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        
		VkDevice device = Aurora::RenderCommand::GetContextAs<Aurora::VulkanContext>()->GetDevice();

		if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create buffer!");
		}
    
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device, buffer, &memRequirements);
    
		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);
    
		if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate buffer memory!");
		}
    
		vkBindBufferMemory(device, buffer, bufferMemory, 0);
	}

	Aurora::VulkanBuffer createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
		const void* data, VkSharingMode sharingMode) {
		
		auto* context = Aurora::VulkanRenderCommand::GetContext();
	    VkDevice device = context->GetDevice();

	    Aurora::VulkanBuffer buffer{};

	    VkBufferCreateInfo bufferCreateInfo{};
	    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	    bufferCreateInfo.size = size;
	    bufferCreateInfo.usage = usage;
	    bufferCreateInfo.sharingMode = sharingMode;

	    if (vkCreateBuffer(device, &bufferCreateInfo, nullptr, &buffer.Handle) != VK_SUCCESS) {
	        // AU_CORE_ASSERT(false, "Failed to create buffer!");
	    }
		
	    VkMemoryRequirements memRequirements;
	    vkGetBufferMemoryRequirements(device, buffer.Handle, &memRequirements);

	    VkMemoryAllocateInfo allocInfo{};
	    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	    allocInfo.allocationSize = memRequirements.size;
	    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	    if (vkAllocateMemory(device, &allocInfo, nullptr, &buffer.Memory) != VK_SUCCESS) {
	        // AU_CORE_ASSERT(false, "Failed to allocate buffer memory!");
	    }

	    vkBindBufferMemory(device, buffer.Handle, buffer.Memory, 0);

		// if data is nullptr, then we don't copy anything
		if (data == nullptr) {
			return buffer;
		}

	    // if not host-visible then staging buffer + copy
	    if (!(properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
	    	AU_CORE_ASSERT(data != nullptr, "No data provided for device local only buffer!");
	    	
	        Aurora::VulkanBuffer stagingBuffer{};

	        VkBufferCreateInfo stagingCreateInfo{};
	        stagingCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	        stagingCreateInfo.size = size;
	        stagingCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	        stagingCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	        VK_CHECK(vkCreateBuffer(device, &stagingCreateInfo, nullptr, &stagingBuffer.Handle));

	        VkMemoryRequirements stagingMemReq;
	        vkGetBufferMemoryRequirements(device, stagingBuffer.Handle, &stagingMemReq);

	        VkMemoryAllocateInfo stagingAllocInfo{};
	        stagingAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	        stagingAllocInfo.allocationSize = stagingMemReq.size;
	        stagingAllocInfo.memoryTypeIndex = findMemoryType(
	            stagingMemReq.memoryTypeBits,
	            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	        );

	        VK_CHECK(vkAllocateMemory(device, &stagingAllocInfo, nullptr, &stagingBuffer.Memory));

	        vkBindBufferMemory(device, stagingBuffer.Handle, stagingBuffer.Memory, 0);
	    	
	        void* mappedMemory;
	        vkMapMemory(device, stagingBuffer.Memory, 0, size, 0, &mappedMemory);
	    	memcpy(mappedMemory, data, static_cast<size_t>(size));
	        vkUnmapMemory(device, stagingBuffer.Memory);
	    	
	        copyBuffer(stagingBuffer.Handle, buffer.Handle, size);
	    	
	        vkDestroyBuffer(device, stagingBuffer.Handle, nullptr);
	        vkFreeMemory(device, stagingBuffer.Memory, nullptr);
	    } else { // Host visible
	    	void* mappedMemory = nullptr;
	    	vkMapMemory(device, buffer.Memory, 0, size, 0, &mappedMemory);

	    	memcpy(mappedMemory, data, static_cast<size_t>(size));

	    	// if it's not host-coherent we need to flush memory
	    	if (!(properties & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
	    		VkMappedMemoryRange range{};
	    		range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	    		range.memory = buffer.Memory;
	    		range.offset = 0;
	    		range.size   = size;
	    		VK_CHECK(vkFlushMappedMemoryRanges(device, 1, &range));
	    	}

	    	vkUnmapMemory(device, buffer.Memory);
	    }

	    return buffer;
	}

	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
		VkDevice device = Aurora::VulkanRenderCommand::GetContext()->GetDevice();
		VkQueue graphicsQueue = Aurora::VulkanRenderCommand::GetContext()->GetGraphicsQueue();
		VkCommandPool commandPool = Aurora::VulkanRenderCommand::GetContext()->GetCommandPool();
		
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = 1;
		
		VkCommandBuffer commandBuffer;
        
		vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		VkBufferCopy copyRegion{};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphicsQueue);

		vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
	}
}
