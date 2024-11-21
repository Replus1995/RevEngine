#pragma once
#include "Rev/Core/Base.h"
#include <vulkan/vulkan.h>

namespace Rev
{

struct FVulkanPhysicalDeviceFeatures
{
	FVulkanPhysicalDeviceFeatures(VkInstance Instance, VkPhysicalDevice PhysicalDevice);

	VkPhysicalDeviceFeatures2 Features{};
	VkPhysicalDeviceSynchronization2Features Synchronization{};
	VkPhysicalDeviceBufferDeviceAddressFeatures BufferDeviceAddress{};
	VkPhysicalDeviceDynamicRenderingFeatures DynamicRendering{};
	VkPhysicalDeviceMultiviewFeatures Multiview{};

};

struct FVulkanPhysicalDeviceProperties
{
	FVulkanPhysicalDeviceProperties(VkInstance Instance, VkPhysicalDevice PhysicalDevice);

	VkPhysicalDeviceProperties2 Properties{};
	VkPhysicalDeviceMultiviewProperties Multiview{};
};


}