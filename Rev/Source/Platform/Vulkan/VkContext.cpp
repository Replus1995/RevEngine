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

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

//#include <VkBootstrap.h>
#include "VkUtils.h"

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

static VkInstance sInstance = VK_NULL_HANDLE;
static VkDevice sDevice = VK_NULL_HANDLE;
static VmaAllocator sAllocator = VMA_NULL;

FVkContext::FVkContext()
{
	for (size_t i = 0; i < 4; i++)
	{
		mClearColor.float32[i] = 0.0f;
		mClearColor.int32[i] = 0;
		mClearColor.uint32[i] = 0;
	}
	mClearDepthStencil.depth = 1.0f;
	mClearDepthStencil.stencil = 0;
}

FVkContext::~FVkContext()
{
}

void FVkContext::Init()
{
	REV_CORE_ASSERT(sInstance == VK_NULL_HANDLE);
	REV_CORE_ASSERT(sDevice == VK_NULL_HANDLE);
	REV_CORE_ASSERT(sAllocator == VMA_NULL);

#ifdef REV_DEBUG
	sVkEnableValidationLayers = true;
#else
	sVkEnableValidationLayers = false;
#endif // REV_DEBUG

	CreateInstance();
	CreateSurface();
	mDevice.PickPhysicalDevice(this);
	mDevice.CreateLogicalDevice(this);
	CreateAllocator();

	mSwapchain.CreateSwapchain(this, &mDevice);
	InitFrameData(mFrameData, REV_VK_FRAME_OVERLAP, &mDevice);

	sInstance = mInstance;
	sDevice = mDevice.GetLogicalDevice();
	sAllocator = mAllocator;
}

void FVkContext::Cleanup()
{
	vkDeviceWaitIdle(sDevice);
	
	sAllocator = VMA_NULL;
	sDevice = VK_NULL_HANDLE;
	sInstance = VK_NULL_HANDLE;

	mMainDeletorQueue.Flush();

	CleanupFrameData(mFrameData, REV_VK_FRAME_OVERLAP, &mDevice);
	mSwapchain.Cleanup(this, &mDevice);

	vmaDestroyAllocator(mAllocator);
	mDevice.Cleanup();
	vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
	vkDestroyInstance(mInstance, nullptr);
}

void FVkContext::BeginFrame()
{
	constexpr uint64 kWaitTime = 1000000000;
	auto& CurFrameData = GetFrameData();
	REV_VK_CHECK(vkWaitForFences(sDevice, 1, &CurFrameData.Fence, true, kWaitTime));
	CurFrameData.DeletorQueue.Flush();
	REV_VK_CHECK(vkResetFences(sDevice, 1, &CurFrameData.Fence));

	REV_VK_CHECK(vkAcquireNextImageKHR(sDevice, mSwapchain.GetSwapchain(), kWaitTime, CurFrameData.SwapchainSemaphore, nullptr, &mCurSwapchainImageIndex));

	mDrawExtent.width = mSwapchain.GetBackImage().Extent.width;
	mDrawExtent.height = mSwapchain.GetBackImage().Extent.height;

	VkCommandBuffer CmdBuffer = CurFrameData.MainCmdBuffer;
	REV_VK_CHECK(vkResetCommandBuffer(CmdBuffer, 0));
	VkCommandBufferBeginInfo CmdBufferBeginInfo = FVkInit::CmdBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	REV_VK_CHECK(vkBeginCommandBuffer(CmdBuffer, &CmdBufferBeginInfo));

	FVkUtils::TransitionImage(CmdBuffer, mSwapchain.GetBackImage().Image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
	
}

void FVkContext::EndFrame()
{
	//end cmd buffer
	auto& CurFrameData = GetFrameData();
	VkCommandBuffer CmdBuffer = CurFrameData.MainCmdBuffer;


	FVkUtils::TransitionImage(CmdBuffer, mSwapchain.GetBackImage().Image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	FVkUtils::TransitionImage(CmdBuffer, mSwapchain.GetImages()[mCurSwapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	FVkUtils::BlitImage(CmdBuffer, mSwapchain.GetBackImage().Image, mSwapchain.GetImages()[mCurSwapchainImageIndex], mDrawExtent, mSwapchain.GetExtent());
	FVkUtils::TransitionImage(CmdBuffer, mSwapchain.GetImages()[mCurSwapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	REV_VK_CHECK(vkEndCommandBuffer(CmdBuffer));

	//submit
	VkCommandBufferSubmitInfo CmdBufferInfo = FVkInit::CmdBufferSubmitInfo(CmdBuffer);
	VkSemaphoreSubmitInfo WaitSemaphoreInfo = FVkInit::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, CurFrameData.SwapchainSemaphore);
	VkSemaphoreSubmitInfo SignalSemaphoreInfo = FVkInit::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, CurFrameData.RenderSemaphore);

	VkSubmitInfo2 SubmitInfo = FVkInit::SubmitInfo(&CmdBufferInfo, &SignalSemaphoreInfo, &WaitSemaphoreInfo);

	//submit command buffer to the queue and execute it.
	//Fence will now block until the graphic commands finish execution
	REV_VK_CHECK(vkQueueSubmit2(mDevice.GetGraphicsQueue(), 1, &SubmitInfo, CurFrameData.Fence));

	//present
	VkPresentInfoKHR PresentInfo{};
	PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	PresentInfo.pNext = nullptr;
	PresentInfo.pSwapchains = &mSwapchain.GetSwapchain();
	PresentInfo.swapchainCount = 1;
	PresentInfo.pWaitSemaphores = &CurFrameData.RenderSemaphore;
	PresentInfo.waitSemaphoreCount = 1;
	PresentInfo.pImageIndices = &mCurSwapchainImageIndex;
	REV_VK_CHECK(vkQueuePresentKHR(mDevice.GetGraphicsQueue(), &PresentInfo));

	mFrameDataIndex = (mFrameDataIndex + 1) % REV_VK_FRAME_OVERLAP;
	//mCurSwapchainImageIndex = 0;

}

void FVkContext::SetClearColor(const Math::FLinearColor& InColor)
{
	for (size_t i = 0; i < 4; i++)
	{
		mClearColor.float32[i] = InColor[i];
	}
}

void FVkContext::SetClearDepthStencil(float InDepth, uint32 InStencil)
{
	mClearDepthStencil.depth = InDepth;
	mClearDepthStencil.stencil = InStencil;
}

void FVkContext::ClearBackBuffer()
{
	auto& CurFrameData = GetFrameData();
	VkCommandBuffer CmdBuffer = CurFrameData.MainCmdBuffer;

	VkImageSubresourceRange ColorImageRange = FVkInit::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);
	vkCmdClearColorImage(CmdBuffer, mSwapchain.GetBackImage().Image, VK_IMAGE_LAYOUT_GENERAL, &mClearColor, 1, &ColorImageRange);
	/*VkImageSubresourceRange DepthImageRange = FVkInit::ImageSubresourceRange(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
	vkCmdClearDepthStencilImage(CmdBuffer, mSwapchain.GetImages()[mCurSwapchainImageIndex], VK_IMAGE_LAYOUT_GENERAL, &mClearDepthStencil, 1, &DepthImageRange);*/
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

void FVkContext::CreateAllocator()
{
	VmaAllocatorCreateInfo AllocatorCreateInfo = {};
	AllocatorCreateInfo.physicalDevice = mDevice.GetPhysicalDevice();
	AllocatorCreateInfo.device = mDevice.GetLogicalDevice();
	AllocatorCreateInfo.instance = mInstance;
	AllocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
	vmaCreateAllocator(&AllocatorCreateInfo, &mAllocator);
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



VkInstance FVkUtils::GetInstance()
{
	return sInstance;
}

VkDevice FVkUtils::GetDevice()
{
	return sDevice;
}

VmaAllocator FVkUtils::GetAllocator()
{
	return sAllocator;
}

}