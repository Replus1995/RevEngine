#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/Deleter.h"
#include <vulkan/vulkan.h>

namespace Rev
{

class FVulkanDevice;
struct FVulkanFrameData
{
public:
	VkCommandPool CmdPool;
	VkCommandBuffer MainCmdBuffer;
	VkSemaphore SwapchainSemaphore;
	VkSemaphore RenderSemaphore;
	VkFence Fence;
	FDeletorQueue DeletorQueue;
};

void InitFrameData(FVulkanFrameData* Frames, uint32 Count, const FVulkanDevice* InDevice);
void CleanupFrameData(FVulkanFrameData* Frames, uint32 Count, const FVulkanDevice* InDevice);

}