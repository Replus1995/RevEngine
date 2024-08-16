#include "VkContext.h"

#include "Rev/Core/Base.h"
#include "Rev/Core/Assert.h"
#include "Rev/Core/Application.h"
#include "Rev/Core/Window.h"

#if defined(REV_PLATFORM_WINDOWS)
#include <windows.h>
#include <vulkan/vulkan_win32.h>
#	define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

//#include <VkBootstrap.h>

namespace Rev
{
namespace
{
static VkDevice sVkDevice = VK_NULL_HANDLE;
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

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& CreateInfo) {
	CreateInfo = {};
	CreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	CreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	CreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	CreateInfo.pfnUserCallback = VkDebugCallback;
	CreateInfo.pUserData = nullptr;
}

}


void FVkContext::Init()
{
#ifdef REV_DEBUG
	sVkEnableValidationLayers = true;
#else
	sVkEnableValidationLayers = false;
#endif // REV_DEBUG

	CreateInstance();
	CreateSurface();
	mDevice.PickPhysicalDevice(this);
	mDevice.CreateLogicalDevice(this);
	mSwapchain.CreateSwapchain(this, &mDevice);
	InitFrameData(mFrameData, REV_VK_FRAME_OVERLAP, &mDevice);
	sVkDevice = mDevice.GetLogicalDevice();
}

void FVkContext::Cleanup()
{
	vkDeviceWaitIdle(sVkDevice);

	sVkDevice = VK_NULL_HANDLE;
	CleanupFrameData(mFrameData, REV_VK_FRAME_OVERLAP, &mDevice);
	mSwapchain.Cleanup(&mDevice);
	mDevice.Cleanup();
	vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
	vkDestroyInstance(mInstance, nullptr);
}

VkDevice FVkContext::GetVkDevice()
{
	return sVkDevice;
}

void FVkContext::BeginFrame()
{
	auto& CurFrameData = GetFrameData();

	REV_VK_CHECK(vkWaitForFences(sVkDevice, 1, &CurFrameData.Fence, true, 1000000000));
	REV_VK_CHECK(vkResetFences(sVkDevice, 1, &CurFrameData.Fence));

	uint32 SwapchainImageIndex;
	REV_VK_CHECK(vkAcquireNextImageKHR(sVkDevice, mSwapchain.GetSwapchain(), 1000000000, CurFrameData.SwapchainSemaphore, nullptr, &SwapchainImageIndex));



	VkCommandBuffer CmdBuffer = CurFrameData.MainCmdBuffer;
	REV_VK_CHECK(vkResetCommandBuffer(CmdBuffer, 0));

	//begin the command buffer recording. We will use this command buffer exactly once, so we want to let vulkan know that
	VkCommandBufferBeginInfo CmdBufferBeginInfo = {};
	CmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	CmdBufferBeginInfo.pNext = nullptr;
	CmdBufferBeginInfo.pInheritanceInfo = nullptr;
	CmdBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	//start the command buffer recording
	REV_VK_CHECK(vkBeginCommandBuffer(CmdBuffer, &CmdBufferBeginInfo));
}

void FVkContext::EndFrame()
{

	mFrameCount++;
}

void FVkContext::CreateInstance()
{
	const std::vector<const char*> EnabledExtensions = GetEnabledExtensions();
	const std::vector<const char*> EnabledLayers = GetEnabledLayers();

	VkApplicationInfo AppInfo{};
	AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	AppInfo.pApplicationName = "RevApp";
	AppInfo.applicationVersion = VK_MAKE_VERSION(1, 3, 0);
	AppInfo.pEngineName = "RevEngine";
	AppInfo.engineVersion = VK_MAKE_VERSION(1, 3, 0);
	AppInfo.apiVersion = VK_API_VERSION_1_3;

	REV_CORE_TRACE("[FVkContext] Vulkan Version {0}.{1}", 1, 3);

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
		PopulateDebugMessengerCreateInfo(DebugMessengerCreateInfo);
		InstanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&DebugMessengerCreateInfo;
	}

	REV_VK_CHECK_THROW(vkCreateInstance(&InstanceCreateInfo, nullptr, &mInstance), "[FVkContext] Failed to create vulkan instance!")
}

void FVkContext::CreateSurface()
{
	Window* pWnd = Application::GetApp().GetWindow();
	REV_CORE_ASSERT(pWnd, "[FVkContext] Invalid window!");

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
		REV_CORE_ASSERT(false, "[FVkContext] Unknown window type!");
	}

	VkWin32SurfaceCreateInfoKHR SurfaceCreateInfo{};
	SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	SurfaceCreateInfo.hwnd = WindoHandle;
	SurfaceCreateInfo.hinstance = GetModuleHandle(nullptr);

	REV_VK_CHECK_THROW(vkCreateWin32SurfaceKHR(mInstance, &SurfaceCreateInfo, nullptr, &mSurface), "[FVkContext] Failed to create window surface!");
#else
	throw std::runtime_error("[FVkContext] Unsupported platform");
#endif
}

void FVkContext::CheckExtensionSupport(const std::vector<const char*>& InExtensionNames)
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
			REV_CORE_ERROR("[FVkContext] Vulkan unsupported extension: {}", InExtensionName);
			throw std::runtime_error("[FVkContext] Vulkan unsupported extension!");
		}
	}

#ifdef REV_DEBUG
	REV_CORE_INFO("[FVkContext] Vulkan enabled extensions:");
	for (const auto& ExtensionName : InExtensionNames) {
		REV_CORE_INFO("{}", ExtensionName);
	}
#endif
}

void FVkContext::CheckLayerSupport(const std::vector<const char*>& InLayerNames)
{
	uint32 AvailableLayerCount = 0;
	vkEnumerateInstanceLayerProperties(&AvailableLayerCount, nullptr);
	std::vector<VkLayerProperties> AvaliableLayers(AvailableLayerCount);
	vkEnumerateInstanceLayerProperties(&AvailableLayerCount, AvaliableLayers.data());

	for (const char* InLayerName : InLayerNames)
	{
		auto iter = std::find_if(AvaliableLayers.begin(), AvaliableLayers.end(), [InLayerName](const VkLayerProperties& Layer){ return strcmp(InLayerName, Layer.layerName) == 0; });
		if (iter == AvaliableLayers.end())
		{
			REV_CORE_ERROR("[FVkContext] Vulkan unsupported layer: {}", InLayerName);
			throw std::runtime_error("[FVkContext] Vulkan unsupported layer!");
		}
	}

#ifdef REV_DEBUG
	REV_CORE_INFO("[FVkContext] Vulkan enabled layers:");
	for (const auto& LayerName : InLayerNames) {
		REV_CORE_INFO("{}", LayerName);
	}
#endif
}

std::vector<const char*> FVkContext::GetEnabledExtensions()
{
	std::vector<const char*> RequiredExtensions;

	Window* pWnd = Application::GetApp().GetWindow();
	REV_CORE_ASSERT(pWnd, "[FVkContext] Invalid window!");
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
		REV_CORE_ASSERT(false, "[FVkContext] Unknown window type!");
	}

	if (sVkEnableValidationLayers)
	{
		RequiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	CheckExtensionSupport(RequiredExtensions);
	return RequiredExtensions;
}

std::vector<const char*> FVkContext::GetEnabledLayers()
{
	std::vector<const char*> RequiredLayers = {};
	
	if (sVkEnableValidationLayers)
	{
		RequiredLayers.push_back("VK_LAYER_KHRONOS_validation");
	}
	
	CheckLayerSupport(RequiredLayers);
	return RequiredLayers;
}





}