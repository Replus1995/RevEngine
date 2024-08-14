#include "VkContext.h"

#include "Rev/Core/Base.h"
#include "Rev/Core/Assert.h"
#include "Rev/Core/Application.h"
#include "Rev/Core/Window.h"

#include <GLFW/glfw3.h>

namespace Rev
{

void FVkContext::Init()
{

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

	Window* pWnd = Application::GetApp().GetWindow();
	RE_CORE_ASSERT(pWnd, "[FVkContext] Invalid window!");
	switch (pWnd->GetType())
	{
	case EWindowType::GLFW:
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		InstanceCreateInfo.enabledExtensionCount = glfwExtensionCount;
		InstanceCreateInfo.ppEnabledExtensionNames = glfwExtensions;
		break;
	}
	default:
		RE_CORE_ASSERT(false, "[FVkContext] Unknown window type!");
	}

	InstanceCreateInfo.enabledLayerCount = 0; //TODO: debug layer

	if (vkCreateInstance(&InstanceCreateInfo, nullptr, &mInstance) != VK_SUCCESS) {
		throw std::runtime_error("[FVkContext] Vulkan create instance failed!");
	}




}

}