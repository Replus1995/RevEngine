#pragma once
#include "Core/VulkanInstance.h"
#include "Core/VulkanDevice.h"
#include "VulkanContext.h"
#include <vk_mem_alloc.h>

namespace Rev
{

class FVulkanContext;
class FVulkanUniformManager;
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
	static const FVulkanSwapChainSupport& GetSwapChainSupport();
	static VmaAllocator GetAllocator();

	static FVulkanContext* GetContext();
	static VkCommandBuffer GetMainCmdBuffer();
	static VkExtent2D GetSwapchainExtent();
	static void ImmediateSubmit(std::function<void(VkCommandBuffer)>&& Func);

	static FVulkanUniformManager* GetUniformManager();
};

}


