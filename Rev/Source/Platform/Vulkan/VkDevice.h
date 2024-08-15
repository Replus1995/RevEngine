#pragma once
#include "Rev/Render/RHI/RHIDevice.h"
#include <vector>
#include <vulkan/vulkan.h>

namespace Rev
{

class FVkContext;
class FVkDevice : public FRHIDevice
{
public:
	void PickPhysicalDevice(const FVkContext* InContext);
	void CreateLogicalDevice();
	virtual void Cleanup();

private:
	bool PhysicalDeviceSuitable(VkPhysicalDevice InDevice);
	std::vector<const char*> GetEnabledExtensions();

private:
	VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
	VkDevice mDevice = VK_NULL_HANDLE;
	VkQueue mGraphicsQueue = VK_NULL_HANDLE;
};

}