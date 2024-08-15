#pragma once
#include "Rev/Render/RHI/RHIDevice.h"
#include <vector>
#include <vulkan/vulkan.h>

namespace Rev
{
struct FVkDeviceSwapChainSupport
{
	VkSurfaceCapabilitiesKHR Capabilities;
	std::vector<VkSurfaceFormatKHR> Formats;
	std::vector<VkPresentModeKHR> PresentModes;
};

class FVkContext;
class FVkDevice : public FRHIDevice
{
public:
	void PickPhysicalDevice(const FVkContext* InContext);
	void CreateLogicalDevice(const FVkContext* InContext);
	virtual void Cleanup();

	const VkPhysicalDevice& GetPhysicalDevice() const { return mPhysicalDevice; }
	const VkDevice& GetLogicalDevice() const { return mDevice; }

	const VkQueue& GetGraphicsQueue() const { return mGraphicsQueue; }
	const VkQueue& GetPresentQueue() const { return mPresentQueue; }

	const FVkDeviceSwapChainSupport& GetSwapChainSupport() const { return mSwapChainSupport; }

private:
	bool PhysicalDeviceSuitable(VkPhysicalDevice InDevice, VkSurfaceKHR InSurface);
	bool CheckExtensionSupport(const std::vector<const char*>& InExtensionNames);
	static const std::vector<const char*>& GetRequiredExtensions();
	static FVkDeviceSwapChainSupport QuerySwapChainSupport(VkPhysicalDevice InDevice, VkSurfaceKHR InSurface);

private:
	VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
	VkDevice mDevice = VK_NULL_HANDLE;

	VkQueue mGraphicsQueue = VK_NULL_HANDLE;
	VkQueue mPresentQueue = VK_NULL_HANDLE;

	FVkDeviceSwapChainSupport mSwapChainSupport;
};

}