#pragma once
#include "Rev/Core/Base.h"
#include <vulkan/vulkan.h>
#include "VkDefines.h"

namespace Rev
{

class FVkInit
{
	//provide simple functions to make vulkan init info
public:
	static VkCommandPoolCreateInfo CmdPoolCreateInfo(uint32 InQueueFamily, VkCommandPoolCreateFlags InFlags = 0);
	static VkCommandBufferAllocateInfo CmdBufferAllocateInfo(VkCommandPool InPool, uint32 InCount = 1);
	static VkCommandBufferBeginInfo CmdBufferBeginInfo(VkCommandBufferUsageFlags InFlags);

	static VkFenceCreateInfo FenceCreateInfo(VkFenceCreateFlags InFlags = 0);
	static VkSemaphoreCreateInfo SemaphoreCreateInfo(VkSemaphoreCreateFlags InFlags = 0);

	static VkImageSubresourceRange ImageSubresourceRange(VkImageAspectFlags InAspectMask);
};

class FVkUtils
{
public:
	static void TransitionImage(VkCommandBuffer CmdBuffer, VkImage Image, VkImageLayout CurrentLayout, VkImageLayout NextLayout);

};

}