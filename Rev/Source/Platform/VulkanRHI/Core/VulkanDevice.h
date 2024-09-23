#pragma once
#include "Rev/Core/Base.h"
#include <vulkan/vulkan.h>

namespace Rev
{

enum EVulkanQueueKind : uint8
{
	VQK_Present = 0,
	VQK_Graphics = 1,
	VQK_Compute = 2,
	VQK_Count = 3
};

struct FVulkanSwapChainSupport
{
	VkSurfaceCapabilitiesKHR Capabilities;
	std::vector<VkSurfaceFormatKHR> Formats;
	std::vector<VkPresentModeKHR> PresentModes;
};

class FVulkanInstance;
class FVulkanDevice
{
public:
	void PickPhysicalDevice(const FVulkanInstance* InInstance);
	void CreateLogicalDevice(const FVulkanInstance* InInstance);
	virtual void Cleanup();

	const VkPhysicalDevice& GetPhysicalDevice() const { return mPhysicalDevice; }
	const VkDevice& GetDevice() const { return mDevice; }

	VkQueue GetQueue(EVulkanQueueKind InKind) const { return mQueues[InKind]; }
	uint32 GetQueueFamily(EVulkanQueueKind InKind) const { return mQueueFamilies[InKind]; }

	const FVulkanSwapChainSupport& GetSwapChainSupport() const { return mSwapChainSupport; }

private:
	static bool PhysicalDeviceSuitable(VkPhysicalDevice InDevice, VkSurfaceKHR InSurface);
	static bool CheckExtensionSupport(VkPhysicalDevice InDevice, const std::vector<const char*>& InExtensionNames);
	static const std::vector<const char*>& GetRequiredExtensions();
	static FVulkanSwapChainSupport QuerySwapChainSupport(VkPhysicalDevice InDevice, VkSurfaceKHR InSurface);

private:
	VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
	VkDevice mDevice = VK_NULL_HANDLE;

	uint32 mQueueFamilies[VQK_Count];
	VkQueue mQueues[VQK_Count];

	FVulkanSwapChainSupport mSwapChainSupport;
};

}