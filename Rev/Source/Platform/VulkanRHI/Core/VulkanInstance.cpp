#include "VulkanInstance.h"
#include "VulkanDeviceFeatures.h"
#include "Rev/Core/Assert.h"
#include "Rev/Core/Application.h"
#include "Rev/Core/Window.h"
#include <set>

#if defined(REV_PLATFORM_WINDOWS)
#include <windows.h>
#include <vulkan/vulkan_win32.h>
#	define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

namespace Rev
{

namespace
{

static bool sVkEnableValidationLayers = false;
static VkDebugUtilsMessageSeverityFlagBitsEXT sVkMinMessageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;

static VKAPI_ATTR VkBool32 VKAPI_CALL VkDebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT MessageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	if (MessageSeverity < sVkMinMessageSeverity)
		return VK_FALSE;

	switch (MessageSeverity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		REV_CORE_VERBOSE("VkDebug: {}", pCallbackData->pMessage);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		REV_CORE_INFO("VkDebug: {}", pCallbackData->pMessage);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		REV_CORE_WARN("VkDebug: {}", pCallbackData->pMessage);
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		REV_CORE_ERROR("VkDebug: {}", pCallbackData->pMessage);
		break;
	}

	return VK_FALSE;
}

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


void FVulkanInstance::CreateInstance()
{
#ifdef REV_DEBUG
	sVkEnableValidationLayers = true;
#else
	sVkEnableValidationLayers = false;
#endif // REV_DEBUG

	const std::vector<const char*> EnabledExtensions = GetEnabledExtensions();
	const std::vector<const char*> EnabledLayers = GetEnabledLayers();

	VkApplicationInfo AppInfo{};
	AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	AppInfo.pApplicationName = "RevApp";
	AppInfo.applicationVersion = VK_MAKE_VERSION(1, 3, 0);
	AppInfo.pEngineName = "RevEngine";
	AppInfo.engineVersion = VK_MAKE_VERSION(1, 3, 0);
	AppInfo.apiVersion = VK_API_VERSION_1_3;

	REV_CORE_TRACE("[FVkInstance] Vulkan Version {0}.{1}", 1, 3);

	VkInstanceCreateInfo InstanceCreateInfo{};
	InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	InstanceCreateInfo.pApplicationInfo = &AppInfo;
	InstanceCreateInfo.enabledExtensionCount = static_cast<uint32>(EnabledExtensions.size());
	InstanceCreateInfo.ppEnabledExtensionNames = EnabledExtensions.empty() ? nullptr : EnabledExtensions.data();
	InstanceCreateInfo.enabledLayerCount = static_cast<uint32>(EnabledLayers.size());
	InstanceCreateInfo.ppEnabledLayerNames = EnabledLayers.empty() ? nullptr : EnabledLayers.data();
	InstanceCreateInfo.pNext = nullptr;

	//DebugCallback
	VkDebugUtilsMessengerCreateInfoEXT DebugMessengerCreateInfo{};
	if (sVkEnableValidationLayers)
	{
		DebugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		DebugMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		DebugMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		DebugMessengerCreateInfo.pfnUserCallback = VkDebugCallback;
		DebugMessengerCreateInfo.pUserData = nullptr;
		InstanceCreateInfo.pNext = &DebugMessengerCreateInfo;
	}

	REV_VK_CHECK_THROW(vkCreateInstance(&InstanceCreateInfo, nullptr, &mInstance), "[FVkInstance] Failed to create vulkan instance!");

	if(sVkEnableValidationLayers)
	{
		VulkanLoadDebugFunctions(mInstance);
	}
}

void FVulkanInstance::CreateSurface()
{
	Window* pWnd = Application::GetApp().GetWindow();
	REV_CORE_ASSERT(pWnd, "[FVkInstance] Invalid window!");

#if defined(REV_PLATFORM_WINDOWS)
	HWND WindoHandle = nullptr;
	switch (pWnd->GetType())
	{
	case EWindowType::GLFW:
	{
		WindoHandle = glfwGetWin32Window(static_cast<GLFWwindow*>(pWnd->GetNativeHandle()));
		break;
	}
	default:
		REV_CORE_ASSERT(false, "[FVkInstance] Unknown window type!");
	}

	VkWin32SurfaceCreateInfoKHR SurfaceCreateInfo{};
	SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	SurfaceCreateInfo.hwnd = WindoHandle;
	SurfaceCreateInfo.hinstance = GetModuleHandle(nullptr);

	REV_VK_CHECK_THROW(vkCreateWin32SurfaceKHR(mInstance, &SurfaceCreateInfo, nullptr, &mSurface), "[FVkInstance] Failed to create window surface!");
#else
	throw std::runtime_error("[FVkInstance] Unsupported platform");
#endif
}

void FVulkanInstance::PickPhysicalDevice()
{
	REV_CORE_ASSERT(mInstance != VK_NULL_HANDLE);
	REV_CORE_ASSERT(mSurface != VK_NULL_HANDLE);

	uint32 PhysicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(mInstance, &PhysicalDeviceCount, nullptr);
	if (PhysicalDeviceCount == 0) {
		throw std::runtime_error("[FVkDevice] Failed to find GPUs with Vulkan support!");
	}
	std::vector<VkPhysicalDevice> PhysicalDevices(PhysicalDeviceCount);
	vkEnumeratePhysicalDevices(mInstance, &PhysicalDeviceCount, PhysicalDevices.data());
	for (const auto& PhysicalDevice : PhysicalDevices) {
		if (PhysicalDeviceSuitable(PhysicalDevice, mSurface)) {
			mPhysicalDevice = PhysicalDevice;
			break;
		}
	}

	if (mPhysicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("[FVkDevice] Failed to find a suitable GPU!");
	}

	FVkQueueFamilyIndices Indices = FindQueueFamilies(mPhysicalDevice, mSurface);
	for (uint8 i = 0; i < VQK_Count; i++)
	{
		mQueueFamilies[i] = Indices[i].value();
	}
	mSurfaceSupport = QuerySurfaceSupport(mPhysicalDevice, mSurface);
}

void FVulkanInstance::CreateLogicalDevice()
{
	REV_CORE_ASSERT(mPhysicalDevice != VK_NULL_HANDLE);

	std::vector<VkDeviceQueueCreateInfo> QueueCreateInfos;
	PopulateQueueCreateInfos(QueueCreateInfos, mQueueFamilies);

	//physical device features
	FVulkanPhysicalDeviceFeatures Features;

	//extenisons
	const std::vector<const char*> EnabledExtensions = GetDeviceRequiredExtensions();


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

void FVulkanInstance::QueryDeviceCapacity(FRHIDeviceCapacity& Capacity)
{
	REV_CORE_ASSERT(mPhysicalDevice != VK_NULL_HANDLE);

	VkPhysicalDeviceProperties DeviceProperties;
	vkGetPhysicalDeviceProperties(mPhysicalDevice, &DeviceProperties);

	VkSampleCountFlags SampleCount = DeviceProperties.limits.framebufferColorSampleCounts & DeviceProperties.limits.framebufferDepthSampleCounts;
	if (SampleCount & VK_SAMPLE_COUNT_64_BIT)
		Capacity.MaxNumSamples = VK_SAMPLE_COUNT_64_BIT;
	else if (SampleCount & VK_SAMPLE_COUNT_32_BIT)
		Capacity.MaxNumSamples = VK_SAMPLE_COUNT_32_BIT;
	else if (SampleCount & VK_SAMPLE_COUNT_16_BIT)
		Capacity.MaxNumSamples = VK_SAMPLE_COUNT_16_BIT;
	else if (SampleCount & VK_SAMPLE_COUNT_8_BIT)
		Capacity.MaxNumSamples = VK_SAMPLE_COUNT_8_BIT;
	else if (SampleCount & VK_SAMPLE_COUNT_4_BIT)
		Capacity.MaxNumSamples = VK_SAMPLE_COUNT_4_BIT;
	else if (SampleCount & VK_SAMPLE_COUNT_2_BIT)
		Capacity.MaxNumSamples = VK_SAMPLE_COUNT_2_BIT;
	else
		Capacity.MaxNumSamples = VK_SAMPLE_COUNT_1_BIT;



}

void FVulkanInstance::Cleanup()
{
	vkDestroyDevice(mDevice, nullptr);
	vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
	vkDestroyInstance(mInstance, nullptr);
}

void FVulkanInstance::CheckExtensionSupport(const std::vector<const char*>& InExtensionNames)
{
	uint32 AvailableExtensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &AvailableExtensionCount, nullptr);
	std::vector<VkExtensionProperties> AvailableExtensions(AvailableExtensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &AvailableExtensionCount, AvailableExtensions.data());

	for (const char* InExtensionName : InExtensionNames)
	{
		auto iter = std::find_if(AvailableExtensions.begin(), AvailableExtensions.end(), [InExtensionName](const VkExtensionProperties& Extension) { return strcmp(InExtensionName, Extension.extensionName) == 0; });
		if (iter == AvailableExtensions.end())
		{
			REV_CORE_ERROR("[FVkInstance] Vulkan unsupported extension: {}", InExtensionName);
			throw std::runtime_error("[FVkInstance] Vulkan unsupported extension!");
		}
	}

#ifdef REV_DEBUG
	REV_CORE_INFO("[FVkInstance] Vulkan enabled extensions:");
	for (const auto& ExtensionName : InExtensionNames) {
		REV_CORE_INFO("{}", ExtensionName);
	}
#endif
}

void FVulkanInstance::CheckLayerSupport(const std::vector<const char*>& InLayerNames)
{
	uint32 AvailableLayerCount = 0;
	vkEnumerateInstanceLayerProperties(&AvailableLayerCount, nullptr);
	std::vector<VkLayerProperties> AvaliableLayers(AvailableLayerCount);
	vkEnumerateInstanceLayerProperties(&AvailableLayerCount, AvaliableLayers.data());

	for (const char* InLayerName : InLayerNames)
	{
		auto iter = std::find_if(AvaliableLayers.begin(), AvaliableLayers.end(), [InLayerName](const VkLayerProperties& Layer) { return strcmp(InLayerName, Layer.layerName) == 0; });
		if (iter == AvaliableLayers.end())
		{
			REV_CORE_ERROR("[FVkInstance] Vulkan unsupported layer: {}", InLayerName);
			throw std::runtime_error("[FVkInstance] Vulkan unsupported layer!");
		}
	}

#ifdef REV_DEBUG
	REV_CORE_INFO("[FVkInstance] Vulkan enabled layers:");
	for (const auto& LayerName : InLayerNames) {
		REV_CORE_INFO("{}", LayerName);
	}
#endif
}

std::vector<const char*> FVulkanInstance::GetEnabledExtensions()
{
	std::vector<const char*> RequiredExtensions = {};

	Window* pWnd = Application::GetApp().GetWindow();
	REV_CORE_ASSERT(pWnd, "[FVkInstance] Invalid window!");
	switch (pWnd->GetType())
	{
	case EWindowType::GLFW:
	{
		uint32 glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		for (uint32 i = 0; i < glfwExtensionCount; i++) {
			RequiredExtensions.emplace_back(glfwExtensions[i]);
		}

		break;
	}
	default:
		REV_CORE_ASSERT(false, "[FVkInstance] Unknown window type!");
	}

	if (sVkEnableValidationLayers)
	{
		RequiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	CheckExtensionSupport(RequiredExtensions);
	return RequiredExtensions;
}

std::vector<const char*> FVulkanInstance::GetEnabledLayers()
{
	std::vector<const char*> RequiredLayers = {};

	if (sVkEnableValidationLayers)
	{
		RequiredLayers.push_back("VK_LAYER_KHRONOS_validation");
	}

	CheckLayerSupport(RequiredLayers);
	return RequiredLayers;
}

bool FVulkanInstance::PhysicalDeviceSuitable(VkPhysicalDevice InDevice, VkSurfaceKHR InSurface)
{
	auto& RequiredExtensionNames = GetDeviceRequiredExtensions();
	bool bExtensionSupported = CheckDeviceExtensionSupport(InDevice, RequiredExtensionNames);

	FVkQueueFamilyIndices Indices = FindQueueFamilies(InDevice, InSurface);

	bool bSurfaceAdequate = false;
	if (bExtensionSupported) {
		FVulkanSurfaceSupport SurfaceSupport = QuerySurfaceSupport(InDevice, InSurface);
		bSurfaceAdequate = !SurfaceSupport.Formats.empty() && !SurfaceSupport.PresentModes.empty();
	}

	return Indices.IsComplete() && bExtensionSupported && bSurfaceAdequate;
}

bool FVulkanInstance::CheckDeviceExtensionSupport(VkPhysicalDevice InDevice, const std::vector<const char*>& InExtensionNames)
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

const std::vector<const char*>& FVulkanInstance::GetDeviceRequiredExtensions()
{
	static std::vector<const char*> RequiredExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
	};

	return RequiredExtensions;
}

FVulkanSurfaceSupport FVulkanInstance::QuerySurfaceSupport(VkPhysicalDevice InDevice, VkSurfaceKHR InSurface)
{
	FVulkanSurfaceSupport Details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(InDevice, InSurface, &Details.Capabilities);

	uint32 FormatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(InDevice, InSurface, &FormatCount, nullptr);
	if (FormatCount != 0) {
		Details.Formats.resize(FormatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(InDevice, InSurface, &FormatCount, Details.Formats.data());
	}

	uint32 PresentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(InDevice, InSurface, &PresentModeCount, nullptr);
	if (PresentModeCount != 0) {
		Details.PresentModes.resize(PresentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(InDevice, InSurface, &PresentModeCount, Details.PresentModes.data());
	}

	return Details;
}

}