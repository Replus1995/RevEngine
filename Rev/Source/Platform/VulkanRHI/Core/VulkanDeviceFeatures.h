#pragma once
#include "Rev/Core/Base.h"
#include <vulkan/vulkan.h>

namespace Rev
{

struct FVulkanPhysicalDeviceFeatures
{
public:
	explicit FVulkanPhysicalDeviceFeatures(VkPhysicalDevice InPhysicalDevice);
	VkPhysicalDeviceFeatures2* Get() { return &mFeatures; }

private:
	VkPhysicalDeviceFeatures2 mFeatures{};
	VkPhysicalDeviceSynchronization2Features mSyncFeatures{};
	VkPhysicalDeviceBufferDeviceAddressFeatures mBufferDeviceAddressFeatures{};
	VkPhysicalDeviceDynamicRenderingFeatures mDynamicRenderingFeatures{};

};

}
