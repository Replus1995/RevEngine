#pragma once
#include "Core/VulkanDefines.h"
#include "Core/VulkanInstance.h"
#include "VulkanContext.h"
#include <vk_mem_alloc.h>

namespace Rev
{

class FVulkanContext;
class FVulkanCore
{
public:
	static void Init();
	static void Cleanup();

	static VkInstance GetInstance();
	static VkSurfaceKHR GetSurface();

	static VkPhysicalDevice GetPhysicalDevice();
	static VkDevice GetDevice();
	static VkQueue GetQueue(EVulkanQueueKind InKind);
	static uint32 GetQueueFamily(EVulkanQueueKind InKind);
	static const FVulkanSurfaceSupport& GetSurfaceSupport();
	static VmaAllocator GetAllocator();

	static FVulkanContext* GetContext();
	static VkCommandBuffer GetMainCmdBuffer();
	static VkExtent2D GetSwapchainExtent();
	static void ImmediateSubmit(std::function<void(VkCommandBuffer)>&& Func);
};

}


