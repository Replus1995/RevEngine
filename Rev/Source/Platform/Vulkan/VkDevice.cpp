#include "VkDevice.h"
#include "VkContext.h"
#include "Rev/Core/Assert.h"
#include <set>

#include "Utils/Device.h"

namespace Rev
{

namespace
{

static FVkQueueFamilyIndices FindQueueFamilies(VkPhysicalDevice InDevice, VkSurfaceKHR InSurface)
{
	FVkQueueFamilyIndices Indices;

	uint32 QueueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(InDevice, &QueueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> QueueFamilies(QueueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(InDevice, &QueueFamilyCount, QueueFamilies.data());

	for (uint32 i = 0; i < QueueFamilies.size(); i++)
	{
		if (QueueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			Indices.GraphicsFamily = i;
		}
		if (QueueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
		{
			Indices.ComputeFamily = i;
		}

		VkBool32 bPresentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(InDevice, i, InSurface, &bPresentSupport);
		if (bPresentSupport)
		{
			Indices.PresentFamily = i;
		}

		if (Indices.IsComplete())
			break;
	}

	return Indices;
}

static void PopulateQueueCreateInfos(std::vector<VkDeviceQueueCreateInfo>& QueueCreateInfos, const FVkQueueFamilyIndices& Indices)
{
	std::set<uint32> UniqueQueueFamilies;
	UniqueQueueFamilies.insert(Indices.GraphicsFamily.value());
	UniqueQueueFamilies.insert(Indices.PresentFamily.value());

	float QueuePriority = 1.0f;
	for (auto QueueFamily : UniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo QueueCreateInfo{};
		QueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		QueueCreateInfo.queueFamilyIndex = QueueFamily;
		QueueCreateInfo.queueCount = 1;
		QueueCreateInfo.pQueuePriorities = &QueuePriority;
		QueueCreateInfos.emplace_back(std::move(QueueCreateInfo));
	}
}


}

bool FVkQueueFamilyIndices::IsComplete()
{
	return GraphicsFamily.has_value() && ComputeFamily.has_value() && PresentFamily.has_value();
}

void FVkDevice::PickPhysicalDevice(const FVkContext* InContext)
{
	REV_CORE_ASSERT(InContext);
	VkInstance InInstance = InContext->GetInstance();
	VkSurfaceKHR InSurface = InContext->GetSurface();

	uint32 PhysicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(InInstance, &PhysicalDeviceCount, nullptr);
	if (PhysicalDeviceCount == 0) {
		throw std::runtime_error("[FVkDevice] Failed to find GPUs with Vulkan support!");
	}
	std::vector<VkPhysicalDevice> PhysicalDevices(PhysicalDeviceCount);
	vkEnumeratePhysicalDevices(InInstance, &PhysicalDeviceCount, PhysicalDevices.data());
	for (const auto& PhysicalDevice : PhysicalDevices) {
		if (PhysicalDeviceSuitable(PhysicalDevice, InSurface)) {
			mPhysicalDevice = PhysicalDevice;
			break;
		}
	}

	if (mPhysicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("[FVkDevice] Failed to find a suitable GPU!");
	}

	mQueueFamilyIndices = FindQueueFamilies(mPhysicalDevice, InSurface);
	mSwapChainSupport = QuerySwapChainSupport(mPhysicalDevice, InSurface);
}

void FVkDevice::CreateLogicalDevice(const FVkContext* InContext)
{
	REV_CORE_ASSERT(InContext);
	REV_CORE_ASSERT(mPhysicalDevice != VK_NULL_HANDLE);
	VkSurfaceKHR InSurface = InContext->GetSurface();

	FVkQueueFamilyIndices Indices = FindQueueFamilies(mPhysicalDevice, InSurface);
	std::vector<VkDeviceQueueCreateInfo> QueueCreateInfos;
	PopulateQueueCreateInfos(QueueCreateInfos, Indices);

	//physical device features
	VkUtils::FVkPhysicalDeviceFeatures DeviceFeatures;

	//extenisons
	const std::vector<const char*> EnabledExtensions = GetRequiredExtensions();


	//device create info
	VkDeviceCreateInfo DeviceCreateInfo{};
	DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	DeviceCreateInfo.pQueueCreateInfos = QueueCreateInfos.data();
	DeviceCreateInfo.queueCreateInfoCount = static_cast<uint32>(QueueCreateInfos.size());
	DeviceCreateInfo.pEnabledFeatures = nullptr;
	DeviceCreateInfo.enabledExtensionCount = static_cast<uint32>(EnabledExtensions.size());
	DeviceCreateInfo.ppEnabledExtensionNames = EnabledExtensions.empty() ? nullptr : EnabledExtensions.data();
	DeviceCreateInfo.enabledLayerCount = 0;
	DeviceCreateInfo.ppEnabledLayerNames = nullptr;
	DeviceCreateInfo.pNext = DeviceFeatures.Get();

	if (vkCreateDevice(mPhysicalDevice, &DeviceCreateInfo, nullptr, &mDevice) != VK_SUCCESS) {
		throw std::runtime_error("[FVkDevice] Failed to create logical device!");
	}

	vkGetDeviceQueue(mDevice, Indices.GraphicsFamily.value(), 0, &mGraphicsQueue);
	vkGetDeviceQueue(mDevice, Indices.PresentFamily.value(), 0, &mPresentQueue);

}

void FVkDevice::Cleanup()
{
	vkDestroyDevice(mDevice, nullptr);
}

bool FVkDevice::PhysicalDeviceSuitable(VkPhysicalDevice InDevice, VkSurfaceKHR InSurface)
{
	/*VkPhysicalDeviceProperties DeviceProperties;
	VkPhysicalDeviceFeatures DeviceFeatures;
	vkGetPhysicalDeviceProperties(InDevice, &DeviceProperties);
	vkGetPhysicalDeviceFeatures(InDevice, &DeviceFeatures);

	return DeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
		DeviceFeatures.geometryShader;*/

	auto& RequiredExtensionNames = GetRequiredExtensions();
	bool bExtensionSupported = CheckExtensionSupport(InDevice, RequiredExtensionNames);

	FVkQueueFamilyIndices Indices = FindQueueFamilies(InDevice, InSurface);

	bool bSwapChainAdequate = false;
	if (bExtensionSupported) {
		FVkDeviceSwapChainSupport SwapChainSupport = QuerySwapChainSupport(InDevice, InSurface);
		bSwapChainAdequate = !SwapChainSupport.Formats.empty() && !SwapChainSupport.PresentModes.empty();
	}

	return Indices.IsComplete() && bExtensionSupported && bSwapChainAdequate;
}

bool FVkDevice::CheckExtensionSupport(VkPhysicalDevice InDevice, const std::vector<const char*>& InExtensionNames)
{
	uint32 AvailableExtensionCount;
	vkEnumerateDeviceExtensionProperties(InDevice, nullptr, &AvailableExtensionCount, nullptr);
	std::vector<VkExtensionProperties> AvailableExtensions(AvailableExtensionCount);
	vkEnumerateDeviceExtensionProperties(InDevice, nullptr, &AvailableExtensionCount, AvailableExtensions.data());

	std::set<std::string> ExtensionnameSet(InExtensionNames.begin(), InExtensionNames.end());
	for (const auto& Extension : AvailableExtensions) {
		ExtensionnameSet.erase(Extension.extensionName);
	}
	return ExtensionnameSet.empty();
}

const std::vector<const char*>& FVkDevice::GetRequiredExtensions()
{
	static std::vector<const char*> RequiredExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	return RequiredExtensions;
}

FVkDeviceSwapChainSupport FVkDevice::QuerySwapChainSupport(VkPhysicalDevice InDevice, VkSurfaceKHR InSurface)
{
	FVkDeviceSwapChainSupport Details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(InDevice, InSurface, &Details.Capabilities);

	uint32 FormatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(InDevice, InSurface, &FormatCount, nullptr);
	if (FormatCount != 0) {
		Details.Formats.resize(FormatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(InDevice, InSurface, &FormatCount, Details.Formats.data());
	}

	uint32 PresentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(InDevice, InSurface, &PresentModeCount, nullptr);
	if (PresentModeCount != 0) {
		Details.PresentModes.resize(PresentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(InDevice, InSurface, &PresentModeCount, Details.PresentModes.data());
	}

	return Details;
}



}


