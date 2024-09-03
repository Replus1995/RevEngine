#include "VkFrameData.h"
#include "Rev/Core/Assert.h"

#include "../VkInitializer.h"
#include "../VkContext.h"
#include "VkDevice.h"

namespace Rev
{

void InitFrameData(FVulkanFrameData* Frames, uint32 Count, const FVulkanDevice* InDevice)
{
	REV_CORE_ASSERT(InDevice);
	VkDevice Device = InDevice->GetLogicalDevice();
	uint32 GraphicsFamily = InDevice->GetQueueFamily(VQK_Graphics);

	VkCommandPoolCreateInfo CmdPoolCreateInfo = FVulkanInit::CmdPoolCreateInfo(GraphicsFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	for (uint32 i = 0; i < Count; i++) {

		REV_VK_CHECK_THROW(vkCreateCommandPool(Device, &CmdPoolCreateInfo, nullptr, &Frames[i].CmdPool), "[FVkFrameData] Failed to create command pool!");

		// allocate the default command buffer that we will use for rendering
		VkCommandBufferAllocateInfo CmdBufferAllocInfo = FVulkanInit::CmdBufferAllocateInfo(Frames[i].CmdPool, 1);
		REV_VK_CHECK_THROW(vkAllocateCommandBuffers(Device, &CmdBufferAllocInfo, &Frames[i].MainCmdBuffer), "[FVkFrameData] Failed to allocate main command buffer!");
	}

	VkFenceCreateInfo FenceCreateInfo = FVulkanInit::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
	VkSemaphoreCreateInfo SemaphoreCreateInf = FVulkanInit::SemaphoreCreateInfo();

	for (uint32 i = 0; i < Count; i++) {
		REV_VK_CHECK_THROW(vkCreateFence(Device, &FenceCreateInfo, nullptr, &Frames[i].Fence), "[FVkFrameData] Failed to create fence!");

		REV_VK_CHECK_THROW(vkCreateSemaphore(Device, &SemaphoreCreateInf, nullptr, &Frames[i].SwapchainSemaphore), "[FVkFrameData] Failed to create swap chain semaphore!");
		REV_VK_CHECK_THROW(vkCreateSemaphore(Device, &SemaphoreCreateInf, nullptr, &Frames[i].RenderSemaphore), "[FVkFrameData] Failed to create render semaphore!");
	}

}

void CleanupFrameData(FVulkanFrameData* Frames, uint32 Count, const FVulkanDevice* InDevice)
{
	REV_CORE_ASSERT(InDevice);
	VkDevice Device = InDevice->GetLogicalDevice();

	for (uint32 i = 0; i < Count; i++) {
		//destroy cmd pool
		vkDestroyCommandPool(Device, Frames[i].CmdPool, nullptr);

		//destroy sync objects
		vkDestroyFence(Device, Frames[i].Fence, nullptr);
		vkDestroySemaphore(Device, Frames[i].RenderSemaphore, nullptr);
		vkDestroySemaphore(Device, Frames[i].SwapchainSemaphore, nullptr);

		Frames[i].DeletorQueue.Flush();
	}
}

}


