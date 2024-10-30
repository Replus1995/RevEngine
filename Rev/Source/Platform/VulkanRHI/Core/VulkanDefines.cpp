#include "VulkanDefines.h"
#include "../VulkanDynamicRHI.h"


VkResult VulkanCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void VulkanDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, messenger, pAllocator);
	}
}

void VulkanSubmitDebugUtilsMessageEXT(VkInstance instance, VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData)
{
	auto func = (PFN_vkSubmitDebugUtilsMessageEXT)vkGetInstanceProcAddr(instance, "vkSubmitDebugUtilsMessageEXT");
	if (func != nullptr) {
		func(instance, messageSeverity, messageTypes, pCallbackData);
	}
}

void VulkanCmdBeginDebugUtilsLabelEXT(VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT* pLabelInfo)
{
	auto func = (PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetInstanceProcAddr(Rev::FVulkanDynamicRHI::GetInstance(), "vkCmdBeginDebugUtilsLabelEXT");
	if (func != nullptr) {
		func(commandBuffer, pLabelInfo);
	}
}

void VulkanCmdEndDebugUtilsLabelEXT(VkCommandBuffer commandBuffer)
{
	auto func = (PFN_vkCmdEndDebugUtilsLabelEXT)vkGetInstanceProcAddr(Rev::FVulkanDynamicRHI::GetInstance(), "vkCmdEndDebugUtilsLabelEXT");
	if (func != nullptr) {
		func(commandBuffer);
	}
}

void VulkanCmdInsertDebugUtilsLabelEXT(VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT* pLabelInfo)
{
	auto func = (PFN_vkCmdInsertDebugUtilsLabelEXT)vkGetInstanceProcAddr(Rev::FVulkanDynamicRHI::GetInstance(), "vkCmdInsertDebugUtilsLabelEXT");
	if (func != nullptr) {
		func(commandBuffer, pLabelInfo);
	}
}