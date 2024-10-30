#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/Assert.h"
#include <vulkan/vulkan.h>

#define REV_VK_CHECK(FUNC_RES) REV_CORE_ASSERT(FUNC_RES == VK_SUCCESS)
#define REV_VK_CHECK_THROW(FUNC_RES, ERR_MSG) if (FUNC_RES != VK_SUCCESS) throw std::runtime_error(ERR_MSG);

#define REV_VK_FRAME_OVERLAP 2
#define REV_VK_RENDERTARGET_SIZE_MAX 8192
#define REV_VK_MAX_SUBPASSES 4


#define REV_VK_LIMIT_SAMPLERS 16
#define REV_VK_LIMIT_TEXTURES 128
#define REV_VK_LIMIT_UNIFORM_BUFFERS 16
#define REV_VK_LIMIT_INPUTS 128

#define REV_VK_MAX_SHADER_UNIFORM_BUFFERS 16
#define REV_VK_MAX_SHADER_UNIFORM_SAMPLERS 16
#define REV_VK_MAX_SHADER_UNIFORMS 32
#define REV_VK_MAX_DESCRIPTORSETS 64


template< class T >
static inline void ZeroVulkanStruct(T& VkStruct, int32 VkStructureType)
{
	(int32&)VkStruct.sType = VkStructureType;
	memset(((uint8*)&VkStruct) + sizeof(VkStructureType), 0, sizeof(T) - sizeof(VkStructureType));
}

VkResult VulkanCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger);
void VulkanDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator);
void VulkanSubmitDebugUtilsMessageEXT(VkInstance instance, VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData);

void VulkanCmdBeginDebugUtilsLabelEXT(VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT* pLabelInfo);
void VulkanCmdEndDebugUtilsLabelEXT(VkCommandBuffer commandBuffer);
void VulkanCmdInsertDebugUtilsLabelEXT(VkCommandBuffer commandBuffer, const VkDebugUtilsLabelEXT* pLabelInfo);