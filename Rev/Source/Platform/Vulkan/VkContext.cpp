#include "VkContext.h"

#include "Rev/Core/Base.h"
#include "Rev/Core/Assert.h"
#include "Rev/Core/Application.h"
#include "Rev/Core/Window.h"

#include <GLFW/glfw3.h>

#include "VkAllocator.h"

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
		REV_CORE_DEBUG("VkDebug: {}", pCallbackData->pMessage);
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

	const std::vector<const char*> EnabledExtensions = GetEnabledExtensions();
	const std::vector<const char*> EnabledLayers = GetEnabledLayers();

    VkApplicationInfo AppInfo{};
    AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    AppInfo.pApplicationName = "RevApp";
    AppInfo.applicationVersion = VK_MAKE_VERSION(1, 2, 0);
    AppInfo.pEngineName = "RevEngine";
    AppInfo.engineVersion = VK_MAKE_VERSION(1, 2, 0);
    AppInfo.apiVersion = VK_API_VERSION_1_2;

	REV_CORE_TRACE("[FVkContext] Vulkan Version {0}.{1}", 1, 2);

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

	if (vkCreateInstance(&InstanceCreateInfo, nullptr, &mInstance) != VK_SUCCESS) {
		throw std::runtime_error("[FVkContext] Vulkan create instance failed!");
	}

}

void FVkContext::Cleanup()
{
	vkDestroyInstance(mInstance, nullptr);
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
		RequiredExtensions.push_back("VK_EXT_DEBUG_UTILS_EXTENSION_NAME");
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