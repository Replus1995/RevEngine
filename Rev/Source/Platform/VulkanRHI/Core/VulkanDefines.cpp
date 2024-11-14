#include "VulkanDefines.h"

PFN_vkCreateDebugUtilsMessengerEXT FNvkCreateDebugUtilsMessengerEXT = nullptr;
PFN_vkDestroyDebugUtilsMessengerEXT FNvkDestroyDebugUtilsMessengerEXT = nullptr;
PFN_vkSubmitDebugUtilsMessageEXT FNvkSubmitDebugUtilsMessageEXT = nullptr;

PFN_vkQueueBeginDebugUtilsLabelEXT FNvkQueueBeginDebugUtilsLabelEXT = nullptr;
PFN_vkQueueEndDebugUtilsLabelEXT FNvkQueueEndDebugUtilsLabelEXT = nullptr;
PFN_vkQueueInsertDebugUtilsLabelEXT FNvkQueueInsertDebugUtilsLabelEXT = nullptr;

PFN_vkCmdBeginDebugUtilsLabelEXT FNvkCmdBeginDebugUtilsLabelEXT = nullptr;
PFN_vkCmdEndDebugUtilsLabelEXT FNvkCmdEndDebugUtilsLabelEXT = nullptr;
PFN_vkCmdInsertDebugUtilsLabelEXT FNvkCmdInsertDebugUtilsLabelEXT = nullptr;

PFN_vkSetDebugUtilsObjectNameEXT FNvkSetDebugUtilsObjectNameEXT = nullptr;
PFN_vkSetDebugUtilsObjectTagEXT FNvkSetDebugUtilsObjectTagEXT = nullptr;


#define CALL_VKDEBUGFUNC(PFN, ...) \
	if(PFN) PFN(__VA_ARGS__)

#define CALL_VKDEBUGFUNC_WITHRETURN(PFN, ...) \
	return  PFN ? PFN(__VA_ARGS__) : VK_ERROR_EXTENSION_NOT_PRESENT

void VulkanLoadDebugFunctions(VkInstance instance)
{
	FNvkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	FNvkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	FNvkSubmitDebugUtilsMessageEXT = (PFN_vkSubmitDebugUtilsMessageEXT)vkGetInstanceProcAddr(instance, "vkSubmitDebugUtilsMessageEXT");

	FNvkQueueBeginDebugUtilsLabelEXT = (PFN_vkQueueBeginDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance, "vkQueueBeginDebugUtilsLabelEXT");
	FNvkQueueEndDebugUtilsLabelEXT = (PFN_vkQueueEndDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance, "vkQueueEndDebugUtilsLabelEXT");
	FNvkQueueInsertDebugUtilsLabelEXT = (PFN_vkQueueInsertDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance, "vkQueueInsertDebugUtilsLabelEXT");

	FNvkCmdBeginDebugUtilsLabelEXT = (PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance, "vkCmdBeginDebugUtilsLabelEXT");
	FNvkCmdEndDebugUtilsLabelEXT = (PFN_vkCmdEndDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance, "vkCmdEndDebugUtilsLabelEXT");
	FNvkCmdInsertDebugUtilsLabelEXT = (PFN_vkCmdInsertDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance, "vkCmdInsertDebugUtilsLabelEXT");

	FNvkSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectNameEXT");
	FNvkSetDebugUtilsObjectTagEXT = (PFN_vkSetDebugUtilsObjectTagEXT)vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectTagEXT");
}

VkResult vkCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger)
{
	CALL_VKDEBUGFUNC_WITHRETURN(FNvkCreateDebugUtilsMessengerEXT, instance, pCreateInfo, pAllocator, pMessenger);
}

void vkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator)
{
	CALL_VKDEBUGFUNC(FNvkDestroyDebugUtilsMessengerEXT, instance, messenger, pAllocator);
}

void vkSubmitDebugUtilsMessageEXT(VkInstance instance, VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData)
{
	CALL_VKDEBUGFUNC(FNvkSubmitDebugUtilsMessageEXT, instance, messageSeverity, messageTypes, pCallbackData);
}

void vkQueueBeginDebugUtilsLabelEXT(VkQueue queue, const VkDebugUtilsLabelEXT* pLabelInfo)
{
	CALL_VKDEBUGFUNC(FNvkQueueBeginDebugUtilsLabelEXT, queue, pLabelInfo);
}

void vkQueueEndDebugUtilsLabelEXT(VkQueue queue)
{
	CALL_VKDEBUGFUNC(FNvkQueueEndDebugUtilsLabelEXT, queue);
}

void vkQueueInsertDebugUtilsLabelEXT(VkQueue queue, const VkDebugUtilsLabelEXT* pLabelInfo)
{
	CALL_VKDEBUGFUNC(FNvkQueueInsertDebugUtilsLabelEXT, queue, pLabelInfo);
}

void vkCmdBeginDebugUtilsLabelEXT(VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT* pLabelInfo)
{
	CALL_VKDEBUGFUNC(FNvkCmdBeginDebugUtilsLabelEXT, commandBuffer, pLabelInfo);
}

void vkCmdEndDebugUtilsLabelEXT(VkCommandBuffer commandBuffer)
{
	CALL_VKDEBUGFUNC(FNvkCmdEndDebugUtilsLabelEXT, commandBuffer);
}

void vkCmdInsertDebugUtilsLabelEXT(VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT* pLabelInfo)
{
	CALL_VKDEBUGFUNC(FNvkCmdInsertDebugUtilsLabelEXT, commandBuffer, pLabelInfo);
}

VkResult vkSetDebugUtilsObjectNameEXT(VkDevice device, const VkDebugUtilsObjectNameInfoEXT* pNameInfo)
{
	CALL_VKDEBUGFUNC_WITHRETURN(FNvkSetDebugUtilsObjectNameEXT, device, pNameInfo);
}

VkResult vkSetDebugUtilsObjectTagEXT(VkDevice device, const VkDebugUtilsObjectTagInfoEXT* pTagInfo)
{
	CALL_VKDEBUGFUNC_WITHRETURN(FNvkSetDebugUtilsObjectTagEXT, device, pTagInfo);
}