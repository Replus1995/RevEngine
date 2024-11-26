#include "VulkanDevice.h"

namespace Rev
{

FVulkanPhysicalDeviceFeatures::FVulkanPhysicalDeviceFeatures(VkInstance Instance, VkPhysicalDevice PhysicalDevice)
{
	Multiview.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES;
	Multiview.multiview = VK_TRUE;
	Multiview.multiviewGeometryShader = VK_TRUE;
	Multiview.multiviewTessellationShader = VK_FALSE;
	Multiview.pNext = nullptr;

	DynamicRendering.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
	DynamicRendering.dynamicRendering = VK_TRUE;
	DynamicRendering.pNext = &Multiview;

	BufferDeviceAddress.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
	BufferDeviceAddress.bufferDeviceAddress = VK_TRUE;
	BufferDeviceAddress.bufferDeviceAddressCaptureReplay = VK_FALSE;
	BufferDeviceAddress.bufferDeviceAddressMultiDevice = VK_FALSE;
	BufferDeviceAddress.pNext = &DynamicRendering;

	Synchronization.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
	Synchronization.synchronization2 = VK_TRUE;
	Synchronization.pNext = &BufferDeviceAddress;

	Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	Features.features = {};
	Features.pNext = &Synchronization;

	//PFN_vkGetPhysicalDeviceFeatures2 vkGetPhysicalDeviceFeatures2 = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures2>(vkGetInstanceProcAddr(Instance, "vkGetPhysicalDeviceFeatures2KHR"));
	vkGetPhysicalDeviceFeatures2(PhysicalDevice, &Features);
}

FVulkanPhysicalDeviceProperties::FVulkanPhysicalDeviceProperties(VkInstance Instance, VkPhysicalDevice PhysicalDevice)
{

	Multiview.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES;
	Multiview.pNext = nullptr;

	Properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
	Properties.pNext = &Multiview;

	vkGetPhysicalDeviceProperties2(PhysicalDevice, &Properties);
}

}