#pragma once
#include "Rev/Core/Base.h"
#include <vector>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Rev
{

struct FVulkanBackImage
{
	VkImage Image;
	VkImageView ImageView;
	VmaAllocation Allocation;
	VkExtent3D Extent;
	VkFormat Format;
};

class FVulkanInstance;
class FVulkanDevice;
class FVulkanSwapchain
{
public:
	void CreateSwapchain(const FVulkanInstance* Instance, const FVulkanDevice* InDevice, VmaAllocator InAllocator);
	void Cleanup(const FVulkanDevice* InDevice, VmaAllocator InAllocator);

	const VkSwapchainKHR& GetSwapchain() const { return mSwapchain; }
	const VkExtent2D& GetExtent() const { return mExtent; }
	const VkFormat& GetFormat() const { return mFormat; }
	const std::vector<VkImage>& GetImages() const { return mImages; }
	const std::vector<VkImageView>& GetImageViews() const { return mImageViews; }
	const FVulkanBackImage& GetBackImage() const { return mBackImage; }

private:
	static VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& InAvailableFormats);
	static VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& InAvailablePresentModes);
	static VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& InCapabilities);
	void CreateImageViews(const FVulkanDevice* InDevice);
	void CreateBackImage(const FVulkanDevice* InDevice, VmaAllocator InAllocator);

private:
	VkSwapchainKHR mSwapchain;
	VkExtent2D mExtent;
	VkFormat mFormat;
	std::vector<VkImage> mImages;
	std::vector<VkImageView> mImageViews;
	//back image
	FVulkanBackImage mBackImage;
};

}