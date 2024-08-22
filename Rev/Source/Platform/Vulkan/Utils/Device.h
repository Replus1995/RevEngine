#pragma once
#include <vulkan/vulkan.h>

namespace Rev::VkUtils
{

struct FVkPhysicalDeviceFeatures
{
public:
	FVkPhysicalDeviceFeatures();
	VkPhysicalDeviceFeatures2* Get() { return &mFeatures; }

private:
	VkPhysicalDeviceFeatures2 mFeatures{};
	VkPhysicalDeviceSynchronization2Features mSyncFeatures{};
	VkPhysicalDeviceBufferDeviceAddressFeatures mBufferDeviceAddressFeatures{};

};

}