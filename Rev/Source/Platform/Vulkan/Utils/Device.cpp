#include "Device.h"

namespace Rev::VkUtils
{

FVkPhysicalDeviceFeatures::FVkPhysicalDeviceFeatures()
{
	mBufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
	mBufferDeviceAddressFeatures.bufferDeviceAddress = true;
	mBufferDeviceAddressFeatures.bufferDeviceAddressCaptureReplay = false;
	mBufferDeviceAddressFeatures.bufferDeviceAddressMultiDevice = true;
	mBufferDeviceAddressFeatures.pNext = nullptr;

	mSyncFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
	mSyncFeatures.synchronization2 = true;
	mSyncFeatures.pNext = &mBufferDeviceAddressFeatures;

	mFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	mFeatures.features = {};
	mFeatures.pNext = &mSyncFeatures;

}

}