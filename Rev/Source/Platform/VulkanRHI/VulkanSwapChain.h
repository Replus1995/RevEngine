#pragma once
#include "Rev/Core/Base.h"
#include <vector>
#include <vulkan/vulkan.h> 

namespace Rev
{

class FVulkanSwapchain
{
public:
	void CreateSwapchain(VkPresentModeKHR InPresentMode);
	void Cleanup();

	const VkSwapchainKHR& GetSwapchain() const { return mSwapchain; }
	const VkExtent2D& GetExtent() const { return mExtent; }
	const VkFormat& GetFormat() const { return mFormat; }
	const std::vector<VkImage>& GetImages() const { return mImages; }
	const std::vector<VkImageView>& GetImageViews() const { return mImageViews; }

private:
	static VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& InAvailableFormats);
	static VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& InAvailablePresentModes, VkPresentModeKHR InTargetPresentMode);
	static VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& InCapabilities);
	void CreateImageViews();

private:
	VkSwapchainKHR mSwapchain;
	VkExtent2D mExtent;
	VkFormat mFormat;
	std::vector<VkImage> mImages;
	std::vector<VkImageView> mImageViews;
};

}