#include "VulkanDevice.h"
#include "VulkanDeviceFeatures.h"
#include "VulkanInstance.h"
#include "Rev/Core/Assert.h"
#include <set>

namespace Rev
{

namespace
{

struct FVkQueueFamilyIndices
{
public:
	std::optional<uint32>& operator[](uint8 Kind)
	{
		return mValues[Kind];
	}

	const std::optional<uint32>& operator[](uint8 Kind) const
	{
		return mValues[Kind];
	}

	bool IsComplete()
	{
		bool bComplete = true;
		for (uint8 i = 0; i < VQK_Count; i++)
		{
			bComplete &= mValues[i].has_value();
		}
		return bComplete;
	}
private:
	std::optional<uint32> mValues[VQK_Count];
};

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
			Indices[VQK_Graphics] = i;
		}
		if (QueueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
		{
			Indices[VQK_Compute] = i;
		}

		VkBool32 bPresentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(InDevice, i, InSurface, &bPresentSupport);
		if (bPresentSupport)
		{
			Indices[VQK_Present] = i;
		}

		if (Indices.IsComplete())
			break;
	}

	return Indices;
}

static void PopulateQueueCreateInfos(std::vector<VkDeviceQueueCreateInfo>& QueueCreateInfos, const uint32* QueueFamilies)
{
	std::set<uint32> UniqueQueueFamilies;
	UniqueQueueFamilies.insert(QueueFamilies[VQK_Graphics]);
	UniqueQueueFamilies.insert(QueueFamilies[VQK_Present]);

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

void FVulkanDevice::PickPhysicalDevice(const FVulkanInstance* InInstance)
{
	REV_CORE_ASSERT(InInstance);
	VkInstance pInstance = InInstance->GetInstance();
	VkSurfaceKHR pSurface = InInstance->GetSurface();

	uint32 PhysicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(pInstance, &PhysicalDeviceCount, nullptr);
	if (PhysicalDeviceCount == 0) {
		throw std::runtime_error("[FVkDevice] Failed to find GPUs with Vulkan support!");
	}
	std::vector<VkPhysicalDevice> PhysicalDevices(PhysicalDeviceCount);
	vkEnumeratePhysicalDevices(pInstance, &PhysicalDeviceCount, PhysicalDevices.data());
	for (const auto& PhysicalDevice : PhysicalDevices) {
		if (PhysicalDeviceSuitable(PhysicalDevice, pSurface)) {
			mPhysicalDevice = PhysicalDevice;
			break;
		}
	}

	if (mPhysicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("[FVkDevice] Failed to find a suitable GPU!");
	}

	FVkQueueFamilyIndices Indices = FindQueueFamilies(mPhysicalDevice, pSurface);
	for (uint8 i = 0; i < VQK_Count; i++)
	{
		mQueueFamilies[i] = Indices[i].value();
	}
	mSwapChainSupport = QuerySwapChainSupport(mPhysicalDevice, pSurface);
}

void FVulkanDevice::CreateLogicalDevice(const FVulkanInstance* InInstance)
{
	REV_CORE_ASSERT(InInstance);
	REV_CORE_ASSERT(mPhysicalDevice != VK_NULL_HANDLE);
	VkSurfaceKHR pSurface = InInstance->GetSurface();

	std::vector<VkDeviceQueueCreateInfo> QueueCreateInfos;
	PopulateQueueCreateInfos(QueueCreateInfos, mQueueFamilies);

	//physical device features
	FVulkanPhysicalDeviceFeatures Features;

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
	DeviceCreateInfo.pNext = Features.Get();

	if (vkCreateDevice(mPhysicalDevice, &DeviceCreateInfo, nullptr, &mDevice) != VK_SUCCESS) {
		throw std::runtime_error("[FVkDevice] Failed to create logical device!");
	}

	vkGetDeviceQueue(mDevice, mQueueFamilies[VQK_Graphics], 0, &mQueues[VQK_Graphics]);
	vkGetDeviceQueue(mDevice, mQueueFamilies[VQK_Present], 0, &mQueues[VQK_Present]);

}

void FVulkanDevice::Cleanup()
{
	vkDestroyDevice(mDevice, nullptr);
}

bool FVulkanDevice::PhysicalDeviceSuitable(VkPhysicalDevice InDevice, VkSurfaceKHR InSurface)
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
		FVulkanSwapChainSupport SwapChainSupport = QuerySwapChainSupport(InDevice, InSurface);
		bSwapChainAdequate = !SwapChainSupport.Formats.empty() && !SwapChainSupport.PresentModes.empty();
	}

	return Indices.IsComplete() && bExtensionSupported && bSwapChainAdequate;
}

bool FVulkanDevice::CheckExtensionSupport(VkPhysicalDevice InDevice, const std::vector<const char*>& InExtensionNames)
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

const std::vector<const char*>& FVulkanDevice::GetRequiredExtensions()
{
	static std::vector<const char*> RequiredExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
	};

	return RequiredExtensions;
}

FVulkanSwapChainSupport FVulkanDevice::QuerySwapChainSupport(VkPhysicalDevice InDevice, VkSurfaceKHR InSurface)
{
	FVulkanSwapChainSupport Details;

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


