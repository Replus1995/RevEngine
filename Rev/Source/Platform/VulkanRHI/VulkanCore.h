#pragma once
#include "VulkanContext.h"

namespace Rev
{

class FVulkanContext;
class FVulkanCore
{
public:
	static VkInstance GetInstance();
	static VkPhysicalDevice GetPhysicalDevice();
	static VkDevice GetDevice();
	static VkQueue GetQueue(EVulkanQueueKind InKind);
	static uint32 GetQueueFamily(EVulkanQueueKind InKind);
	static VmaAllocator GetAllocator();
	static VkCommandBuffer GetMainCmdBuffer();

	static void ImmediateSubmit(std::function<void(VkCommandBuffer)>&& Func);

private:
	static FVulkanContext* GetContext();
};

}


