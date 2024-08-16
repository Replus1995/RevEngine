#pragma once
#include "Rev/Render/RHI/RHISwapchain.h"
#include <vector>
#include <vulkan/vulkan.h>

namespace Rev
{

class FVkContext;
class FVkDevice;
class FVkSwapchain : public FRHISwapchain
{
public:
	void CreateSwapchain(const FVkContext* InContext, const FVkDevice* InDevice);
	void Cleanup(const FVkDevice* InDevice);

	const VkSwapchainKHR& GetSwapchain() const { return mSwapchain; }
	const VkExtent2D& GetExtent() const { return mExtent; }
	const VkFormat& GetFormat() const { return mFormat; }

private:
	static VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& InAvailableFormats);
	static VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& InAvailablePresentModes);
	static VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& InCapabilities);
	void CreateImageViews(const FVkDevice* InDevice);

private:
	VkSwapchainKHR mSwapchain;
	VkExtent2D mExtent;
	VkFormat mFormat;
	std::vector<VkImage> mImages;
	std::vector<VkImageView> mImageViews;
};

}