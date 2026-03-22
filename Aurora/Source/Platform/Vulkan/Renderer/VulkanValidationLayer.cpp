#include "aupch.h"

#include "Platform/Vulkan/Renderer/VulkanValidationLayer.h"
#include "Aurora/Core/Log.h"

#include <cstdlib>
#include <sstream>

namespace Aurora {
	bool VulkanValidationLayer::checkValidationLayerSupport() {
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayers) {
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				return false;
			}
		}

		return true;
	}

	VkResult VulkanValidationLayer::CreateDebugUtilsMessengerEXT(
		VkInstance instance, 
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
		const VkAllocationCallbacks* pAllocator, 
		VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, 
																			   "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		} else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void VulkanValidationLayer::DestroyDebugUtilsMessengerEXT(VkInstance instance, const VkAllocationCallbacks* pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, 
																				"vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, m_DebugMessenger, pAllocator);
		}
	}

	VkBool32 VulkanValidationLayer::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {
		
		std::ostringstream ss;
		
		if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
			ss << "[ERROR] [VULKAN] Validation Layer:" << pCallbackData->pMessage;
			AU_CORE_ERROR(ss.str());
			return VK_FALSE;
		}
		if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
			ss << "[WARNING] [VULKAN] Validation Layer:" << pCallbackData->pMessage;
			AU_CORE_WARN(ss.str());
			return VK_FALSE;
		}
		if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
			ss << "[INFO] [VULKAN] Validation Layer:" << pCallbackData->pMessage;
			AU_CORE_TRACE(ss.str());
			return VK_FALSE;
		} 

		ss << "[VERBOSE] [VULKAN] Validation Layer: " << pCallbackData->pMessage;
		AU_CORE_INFO(ss.str());
		
		return VK_FALSE;
	}

	void VulkanValidationLayer::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		
		createInfo.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			// VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		
		createInfo.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		
		createInfo.pfnUserCallback = debugCallback;
	}

	void VulkanValidationLayer::setupDebugMessenger(const VkInstance& instance) {

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		populateDebugMessengerCreateInfo(createInfo);

		if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS) {
			throw std::runtime_error("failed to set up debug messenger!");
		}
	}

	void VulkanValidationLayer::addValidationFeaturesToCreateInfo(VkInstanceCreateInfo& createInfo) {
		static std::vector<VkValidationFeatureEnableEXT> enabledFeatures = {
			VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
			VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT,
			VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_RESERVE_BINDING_SLOT_EXT,
			VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT,
			// VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT
		};

		static VkValidationFeaturesEXT features {};
		features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
		features.pNext = nullptr;
		features.enabledValidationFeatureCount = enabledFeatures.size();
		features.pEnabledValidationFeatures = enabledFeatures.data();
		features.disabledValidationFeatureCount = 0;
		features.pDisabledValidationFeatures = nullptr;

		static VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};
		debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugCreateInfo.pNext = nullptr;
		debugCreateInfo.flags = 0;
		debugCreateInfo.messageSeverity =
			// VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugCreateInfo.messageType =
			// VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			// VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugCreateInfo.pfnUserCallback = debugCallback;
		debugCreateInfo.pUserData = nullptr;

		debugCreateInfo.pNext = createInfo.pNext;
		features.pNext = &debugCreateInfo;
		createInfo.pNext = &features;
	}
}
