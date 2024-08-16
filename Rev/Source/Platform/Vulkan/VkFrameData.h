#pragma once
#include "Rev/Core/Base.h"
#include <vulkan/vulkan.h>
#include "VkDefines.h"

namespace Rev
{

class FVkDevice;
struct FVkFrameData
{
public:
	VkCommandPool CmdPool;
	VkCommandBuffer MainCmdBuffer;
	VkSemaphore SwapchainSemaphore;
	VkSemaphore RenderSemaphore;
	VkFence Fence;

};

void InitFrameData(FVkFrameData* Frames, uint32 Count, const FVkDevice* InDevice);
void CleanupFrameData(FVkFrameData* Frames, uint32 Count, const FVkDevice* InDevice);

}