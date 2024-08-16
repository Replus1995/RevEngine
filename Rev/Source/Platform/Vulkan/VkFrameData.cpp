#include "VkFrameData.h"
#include "Rev/Core/Assert.h"

#include "VkContext.h"

namespace Rev
{

void InitFrameData(FVkFrameData* Frames, uint32 Count, const FVkDevice* InDevice)
{
	REV_CORE_ASSERT(InDevice);
	VkDevice Device = InDevice->GetLogicalDevice();

	VkCommandPoolCreateInfo CommandPoolCreateInfo = {};
	CommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	CommandPoolCreateInfo.pNext = nullptr;
	CommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	CommandPoolCreateInfo.queueFamilyIndex = InDevice->GetQueueFamilyIndices().GraphicsFamily.value();

	for (uint32 i = 0; i < Count; i++) {

		REV_VK_CHECK_THROW(vkCreateCommandPool(Device, &CommandPoolCreateInfo, nullptr, &Frames[i].CmdPool), "[FVkFrameData] Failed to create command pool!");

		// allocate the default command buffer that we will use for rendering
		VkCommandBufferAllocateInfo CommandBufferAllocInfo = {};
		CommandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		CommandBufferAllocInfo.pNext = nullptr;
		CommandBufferAllocInfo.commandPool = Frames[i].CmdPool;
		CommandBufferAllocInfo.commandBufferCount = 1;
		CommandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		REV_VK_CHECK_THROW(vkAllocateCommandBuffers(Device, &CommandBufferAllocInfo, &Frames[i].MainCmdBuffer), "[FVkFrameData] Failed to allocate main command buffer!");
	}

	VkFenceCreateInfo FenceCreateInfo = {};
	FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	FenceCreateInfo.pNext = nullptr;
	FenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;


	VkSemaphoreCreateInfo SemaphoreCreateInf = {};
	SemaphoreCreateInf.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	SemaphoreCreateInf.pNext = nullptr;
	SemaphoreCreateInf.flags = 0;

	for (uint32 i = 0; i < Count; i++) {
		REV_VK_CHECK_THROW(vkCreateFence(Device, &FenceCreateInfo, nullptr, &Frames[i].Fence), "[FVkFrameData] Failed to create fence!");

		REV_VK_CHECK_THROW(vkCreateSemaphore(Device, &SemaphoreCreateInf, nullptr, &Frames[i].SwapchainSemaphore), "[FVkFrameData] Failed to create swap chain semaphore!");
		REV_VK_CHECK_THROW(vkCreateSemaphore(Device, &SemaphoreCreateInf, nullptr, &Frames[i].RenderSemaphore), "[FVkFrameData] Failed to create render semaphore!");
	}

}

void CleanupFrameData(FVkFrameData* Frames, uint32 Count, const FVkDevice* InDevice)
{
	REV_CORE_ASSERT(InDevice);
	VkDevice Device = InDevice->GetLogicalDevice();

	for (uint32 i = 0; i < Count; i++) {
		vkDestroyCommandPool(Device, Frames[i].CmdPool, nullptr);
	}
}

}


