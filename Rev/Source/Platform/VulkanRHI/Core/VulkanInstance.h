#pragma once
#include "Rev/Core/Base.h"
#include "VulkanDefines.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Rev
{

enum EVulkanQueueKind : uint8
{
	VQK_Present = 0,
	VQK_Graphics = 1,
	VQK_Compute = 2,
	VQK_Count = 3
};

struct FVulkanSurfaceSupport
{
	VkSurfaceCapabilitiesKHR Capabilities;
	std::vector<VkSurfaceFormatKHR> Formats;
	std::vector<VkPresentModeKHR> PresentModes;
};

class FVulkanInstance
{
public:
	void CreateInstance();
	void CreateSurface();
	void PickPhysicalDevice();
	void CreateLogicalDevice();
	void Cleanup();

	const VkInstance& GetInstance() const { return mInstance; }
	const VkSurfaceKHR& GetSurface() const { return mSurface; }

	const VkPhysicalDevice& GetPhysicalDevice() const { return mPhysicalDevice; }
	const VkDevice& GetDevice() const { return mDevice; }

	VkQueue GetQueue(EVulkanQueueKind InKind) const { return mQueues[InKind]; }
	uint32 GetQueueFamily(EVulkanQueueKind InKind) const { return mQueueFamilies[InKind]; }

	const FVulkanSurfaceSupport& GetSurfaceSupport() const { return mSurfaceSupport; }

private:
	static void CheckExtensionSupport(const std::vector<const char*>& InExtensionNames);
	static void CheckLayerSupport(const std::vector<const char*>& InLayerNames);
	static std::vector<const char*> GetEnabledExtensions();
	static std::vector<const char*> GetEnabledLayers();

	static bool PhysicalDeviceSuitable(VkPhysicalDevice InDevice, VkSurfaceKHR InSurface);
	static bool CheckDeviceExtensionSupport(VkPhysicalDevice InDevice, const std::vector<const char*>& InExtensionNames);
	static const std::vector<const char*>& GetDeviceRequiredExtensions();
	static FVulkanSurfaceSupport QuerySurfaceSupport(VkPhysicalDevice InDevice, VkSurfaceKHR InSurface);

private:
	VkInstance mInstance = VK_NULL_HANDLE;
	//VkDebugUtilsMessengerEXT mDebugMessenger = VK_NULL_HANDLE;
	VkSurfaceKHR mSurface = VK_NULL_HANDLE;

	VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
	VkDevice mDevice = VK_NULL_HANDLE;

	uint32 mQueueFamilies[VQK_Count];
	VkQueue mQueues[VQK_Count];

	FVulkanSurfaceSupport mSurfaceSupport;
};

}