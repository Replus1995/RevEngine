#pragma once
#include "Rev/Core/Base.h"
#include <vulkan/vulkan.h>

namespace Rev
{

class FVulkanInit
{
	//provide simple functions to make vulkan init info
public:
	static VkCommandPoolCreateInfo CmdPoolCreateInfo(uint32 InQueueFamily, VkCommandPoolCreateFlags InFlags = 0);
	static VkCommandBufferAllocateInfo CmdBufferAllocateInfo(VkCommandPool InPool, uint32 InCount = 1);
	static VkCommandBufferBeginInfo CmdBufferBeginInfo(VkCommandBufferUsageFlags InFlags);
	static VkCommandBufferSubmitInfo CmdBufferSubmitInfo(VkCommandBuffer InCmdBuffer);

	static VkFenceCreateInfo FenceCreateInfo(VkFenceCreateFlags InFlags = 0);
	static VkSemaphoreCreateInfo SemaphoreCreateInfo(VkSemaphoreCreateFlags InFlags = 0);
	static VkSemaphoreSubmitInfo SemaphoreSubmitInfo(VkPipelineStageFlags2 InStageMask, VkSemaphore InSemaphore);

	static VkSubmitInfo2 SubmitInfo(VkCommandBufferSubmitInfo* CmdBufferInfo, VkSemaphoreSubmitInfo* SignalSemaphoreInfo, VkSemaphoreSubmitInfo* WaitSemaphoreInfo);
};

}