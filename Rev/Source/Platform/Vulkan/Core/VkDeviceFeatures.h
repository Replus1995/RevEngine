#pragma once
#include "Rev/Core/Base.h"
#include <vulkan/vulkan.h>

namespace Rev
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