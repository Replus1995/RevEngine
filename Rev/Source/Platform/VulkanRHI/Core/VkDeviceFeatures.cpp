#include "VkDeviceFeatures.h"

namespace Rev
{

FVkPhysicalDeviceFeatures::FVkPhysicalDeviceFeatures()
{
	mDynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
	mDynamicRenderingFeatures.dynamicRendering = VK_TRUE;
	mDynamicRenderingFeatures.pNext = nullptr;

	mBufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
	mBufferDeviceAddressFeatures.bufferDeviceAddress = VK_TRUE;
	mBufferDeviceAddressFeatures.bufferDeviceAddressCaptureReplay = VK_FALSE;
	mBufferDeviceAddressFeatures.bufferDeviceAddressMultiDevice = VK_FALSE;
	mBufferDeviceAddressFeatures.pNext = &mDynamicRenderingFeatures;

	mSyncFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
	mSyncFeatures.synchronization2 = VK_TRUE;
	mSyncFeatures.pNext = &mBufferDeviceAddressFeatures;

	mFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	mFeatures.features = {};
	mFeatures.pNext = &mSyncFeatures;

}

}