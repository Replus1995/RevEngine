#pragma once
#include "Rev/Core/Base.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Rev
{

class FVulkanInstance
{
public:
	void CreateInstance();
	void CreateSurface();
	void Cleanup();

	const VkInstance& GetInstance() const { return mInstance; }
	const VkSurfaceKHR& GetSurface() const { return mSurface; }
private:
	static void CheckExtensionSupport(const std::vector<const char*>& InExtensionNames);
	static void CheckLayerSupport(const std::vector<const char*>& InLayerNames);
	static std::vector<const char*> GetEnabledExtensions();
	static std::vector<const char*> GetEnabledLayers();

private:
	VkInstance mInstance = VK_NULL_HANDLE;
	//VkDebugUtilsMessengerEXT mDebugMessenger = VK_NULL_HANDLE;
	VkSurfaceKHR mSurface = VK_NULL_HANDLE;
};

}