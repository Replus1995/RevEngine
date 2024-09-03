#pragma once
#include "Rev/Render/RHI/RHIDevice.h"
#include <vector>
#include <optional>
#include <vulkan/vulkan.h>

namespace Rev
{

enum EVkQueueKind : uint8
{
	VQK_Present = 0,
	VQK_Graphics = 1,
	VQK_Compute = 2,
	VQK_Count = 3
};

struct FVkDeviceSwapChainSupport
{
	VkSurfaceCapabilitiesKHR Capabilities;
	std::vector<VkSurfaceFormatKHR> Formats;
	std::vector<VkPresentModeKHR> PresentModes;
};

class FVkInstance;
class FVkDevice : public FRHIDevice
{
public:
	void PickPhysicalDevice(const FVkInstance* InInstance);
	void CreateLogicalDevice(const FVkInstance* InInstance);
	virtual void Cleanup();

	const VkPhysicalDevice& GetPhysicalDevice() const { return mPhysicalDevice; }
	const VkDevice& GetLogicalDevice() const { return mDevice; }

	VkQueue GetQueue(EVkQueueKind InKind) const { return mQueues[InKind]; }
	uint32 GetQueueFamily(EVkQueueKind InKind) const { return mQueueFamilies[InKind]; }

	const FVkDeviceSwapChainSupport& GetSwapChainSupport() const { return mSwapChainSupport; }

private:
	static bool PhysicalDeviceSuitable(VkPhysicalDevice InDevice, VkSurfaceKHR InSurface);
	static bool CheckExtensionSupport(VkPhysicalDevice InDevice, const std::vector<const char*>& InExtensionNames);
	static const std::vector<const char*>& GetRequiredExtensions();
	static FVkDeviceSwapChainSupport QuerySwapChainSupport(VkPhysicalDevice InDevice, VkSurfaceKHR InSurface);

private:
	VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
	VkDevice mDevice = VK_NULL_HANDLE;

	uint32 mQueueFamilies[VQK_Count];
	VkQueue mQueues[VQK_Count];

	FVkDeviceSwapChainSupport mSwapChainSupport;
};

}