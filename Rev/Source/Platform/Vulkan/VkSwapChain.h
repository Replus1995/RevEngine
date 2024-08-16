#pragma once
#include "Rev/Render/RHI/RHISwapChain.h"
#include <vector>
#include <vulkan/vulkan.h>

namespace Rev
{

class FVkContext;
class FVkDevice;
class FVkSwapChain : public FRHISwapChain
{
public:
	void CreateSwapChain(const FVkContext* InContext, const FVkDevice* InDevice);
	void Cleanup(const FVkDevice* InDevice);

private:
	static VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& InAvailableFormats);
	static VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& InAvailablePresentModes);
	static VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& InCapabilities);
private:
	VkSwapchainKHR mSwapChain;
};

}