#include "VkContext.h"

#include "Rev/Core/Base.h"
#include "Rev/Core/Assert.h"
#include "Rev/Core/Application.h"
#include "Rev/Core/Window.h"

#include <GLFW/glfw3.h>

#include "VkAllocator.h"

namespace Rev
{

void FVkContext::Init()
{
	std::vector<const char*> RequiredExtensions;

	Window* pWnd = Application::GetApp().GetWindow();
	RE_CORE_ASSERT(pWnd, "[FVkContext] Invalid window!");
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
		RE_CORE_ASSERT(false, "[FVkContext] Unknown window type!");
	}

    VkApplicationInfo AppInfo{};
    AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    AppInfo.pApplicationName = "RevApp";
    AppInfo.applicationVersion = VK_MAKE_VERSION(1, 2, 0);
    AppInfo.pEngineName = "RevEngine";
    AppInfo.engineVersion = VK_MAKE_VERSION(1, 2, 0);
    AppInfo.apiVersion = VK_API_VERSION_1_2;

	RE_CORE_TRACE("[FVkContext] Vulkan Version {0}.{1}", 1, 2);

    VkInstanceCreateInfo InstanceCreateInfo{};
    InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    InstanceCreateInfo.pApplicationInfo = &AppInfo;
	InstanceCreateInfo.enabledExtensionCount = RequiredExtensions.size();
	InstanceCreateInfo.ppEnabledExtensionNames = RequiredExtensions.data();
	InstanceCreateInfo.enabledLayerCount = 0; //TODO: debug layer

	if (vkCreateInstance(&InstanceCreateInfo, nullptr, &mInstance) != VK_SUCCESS) {
		throw std::runtime_error("[FVkContext] Vulkan create instance failed!");
	}
}

void FVkContext::Cleanup()
{
	vkDestroyInstance(mInstance, nullptr);
}

void FVkContext::QueryExtensionSupport()
{
	uint32 ExtensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, nullptr);
	std::vector<VkExtensionProperties> Extensions(ExtensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, Extensions.data());

	RE_CORE_INFO("[FVkContext] Vulkan available extensions:");
	for (const auto& Ext : Extensions) {
		RE_CORE_INFO("{}", Ext.extensionName);
	}
}

std::vector<const char*> FVkContext::CheckValidationLayerSupport(const std::vector<const char*>& InValidationLayers)
{
	return std::vector<const char*>();
}


}