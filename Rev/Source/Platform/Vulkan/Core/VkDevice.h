#pragma once
#include "Rev/Render/RHI/RHIDevice.h"
#include <vector>
#include <optional>
#include <vulkan/vulkan.h>

namespace Rev
{

struct FVkQueueFamilyIndices
{
	std::optional<uint32> GraphicsFamily;
	std::optional<uint32> ComputeFamily;

	std::optional<uint32> PresentFamily;

	bool IsComplete();
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

	const FVkQueueFamilyIndices& GetQueueFamilyIndices() const { return mQueueFamilyIndices; }
	const FVkDeviceSwapChainSupport& GetSwapChainSupport() const { return mSwapChainSupport; }

	const VkQueue& GetGraphicsQueue() const { return mGraphicsQueue; }
	const VkQueue& GetPresentQueue() const { return mPresentQueue; }

private:
	static bool PhysicalDeviceSuitable(VkPhysicalDevice InDevice, VkSurfaceKHR InSurface);
	static bool CheckExtensionSupport(VkPhysicalDevice InDevice, const std::vector<const char*>& InExtensionNames);
	static const std::vector<const char*>& GetRequiredExtensions();
	static FVkDeviceSwapChainSupport QuerySwapChainSupport(VkPhysicalDevice InDevice, VkSurfaceKHR InSurface);

private:
	VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
	VkDevice mDevice = VK_NULL_HANDLE;

	FVkQueueFamilyIndices mQueueFamilyIndices;
	FVkDeviceSwapChainSupport mSwapChainSupport;

	VkQueue mGraphicsQueue = VK_NULL_HANDLE;
	VkQueue mPresentQueue = VK_NULL_HANDLE;

};

}