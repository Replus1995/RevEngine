#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/Deleter.h"
#include <vulkan/vulkan.h>
#include "Core/VulkanDescriptor.h"

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

	FVulkanDescriptorPool DescriptorPool;
};

void InitFrameData(FVulkanFrameData* Frames, uint32 Count);
void CleanupFrameData(FVulkanFrameData* Frames, uint32 Count);

}