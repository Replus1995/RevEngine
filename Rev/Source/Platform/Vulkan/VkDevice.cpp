#include "VkDevice.h"
#include "VkContext.h"
#include "Rev/Core/Assert.h"
#include <optional>

namespace Rev
{

namespace
{

struct FVkQueueFamilyIndices
{
	std::optional<uint32> GraphicsFamily;
	std::optional<uint32> ComputeFamily;
	std::optional<uint32> TransferFamily;

	bool IsComplete()
	{
		return GraphicsFamily.has_value() && ComputeFamily.has_value() && TransferFamily.has_value();
	}
};


static FVkQueueFamilyIndices FindQueueFamilies(VkPhysicalDevice InDevice)
{
	FVkQueueFamilyIndices Indices;

	uint32 QueueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(InDevice, &QueueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> QueueFamilies(QueueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(InDevice, &QueueFamilyCount, QueueFamilies.data());

	for (size_t i = 0; i < QueueFamilies.size(); i++)
	{
		if (QueueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			Indices.GraphicsFamily = i;
		}
		if (QueueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
		{
			Indices.ComputeFamily = i;
		}
		if (QueueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
		{
			Indices.TransferFamily = i;
		}

		if (Indices.IsComplete())
			break;
	}

	return Indices;
}

}


void FVkDevice::PickPhysicalDevice(const FVkContext* InContext)
{
	REV_CORE_ASSERT(InContext);
	VkInstance InInstance = InContext->GetInstance();

	uint32 PhysicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(InInstance, &PhysicalDeviceCount, nullptr);
	if (PhysicalDeviceCount == 0) {
		throw std::runtime_error("[FVkDevice] Failed to find GPUs with Vulkan support!");
	}
	std::vector<VkPhysicalDevice> PhysicalDevices(PhysicalDeviceCount);
	vkEnumeratePhysicalDevices(InInstance, &PhysicalDeviceCount, PhysicalDevices.data());
	for (const auto& PhysicalDevice : PhysicalDevices) {
		if (PhysicalDeviceSuitable(PhysicalDevice)) {
			mPhysicalDevice = PhysicalDevice;
			break;
		}
	}

	if (mPhysicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("[FVkDevice] Failed to find a suitable GPU!");
	}
}

void FVkDevice::CreateLogicalDevice()
{
	REV_CORE_ASSERT(mPhysicalDevice != VK_NULL_HANDLE);
	FVkQueueFamilyIndices Indices = FindQueueFamilies(mPhysicalDevice);
	std::vector<VkDeviceQueueCreateInfo> QueueCreateInfos;
	{
		//queue create info
		float GraphicsQueuePriority = 1.0f;
		VkDeviceQueueCreateInfo GraphicsQueueCreateInfo{};
		GraphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		GraphicsQueueCreateInfo.queueFamilyIndex = Indices.GraphicsFamily.value();
		GraphicsQueueCreateInfo.queueCount = 1;
		GraphicsQueueCreateInfo.pQueuePriorities = &GraphicsQueuePriority;

		QueueCreateInfos.emplace_back(std::move(GraphicsQueueCreateInfo));
	}

	//physical device features
	VkPhysicalDeviceFeatures PhysicalDeviceFeatures{};

	//extenisons
	const std::vector<const char*> EnabledExtensions = GetEnabledExtensions();

	//device create info
	VkDeviceCreateInfo DeviceCreateInfo{};
	DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	DeviceCreateInfo.pQueueCreateInfos = QueueCreateInfos.data();
	DeviceCreateInfo.queueCreateInfoCount = static_cast<uint32>(QueueCreateInfos.size());
	DeviceCreateInfo.pEnabledFeatures = &PhysicalDeviceFeatures;
	DeviceCreateInfo.enabledExtensionCount = static_cast<uint32>(EnabledExtensions.size());
	DeviceCreateInfo.ppEnabledExtensionNames = EnabledExtensions.empty() ? nullptr : EnabledExtensions.data();
	DeviceCreateInfo.enabledLayerCount = 0;
	DeviceCreateInfo.ppEnabledLayerNames = nullptr;

	if (vkCreateDevice(mPhysicalDevice, &DeviceCreateInfo, nullptr, &mDevice) != VK_SUCCESS) {
		throw std::runtime_error("[FVkDevice] Failed to create logical device!");
	}

	vkGetDeviceQueue(mDevice, Indices.GraphicsFamily.value(), 0, &mGraphicsQueue);

}

void FVkDevice::Cleanup()
{
	vkDestroyDevice(mDevice, nullptr);
}

bool FVkDevice::PhysicalDeviceSuitable(VkPhysicalDevice InDevice)
{
	/*VkPhysicalDeviceProperties DeviceProperties;
	VkPhysicalDeviceFeatures DeviceFeatures;
	vkGetPhysicalDeviceProperties(InDevice, &DeviceProperties);
	vkGetPhysicalDeviceFeatures(InDevice, &DeviceFeatures);

	return DeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
		DeviceFeatures.geometryShader;*/
	FVkQueueFamilyIndices Indices = FindQueueFamilies(InDevice);
	return Indices.IsComplete();
}

std::vector<const char*> FVkDevice::GetEnabledExtensions()
{
	std::vector<const char*> RequiredExtensions;

	RequiredExtensions.push_back("VK_KHR_swapchain");

	return RequiredExtensions;
}

}


