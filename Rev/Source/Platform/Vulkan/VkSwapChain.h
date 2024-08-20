#pragma once
#include "Rev/Render/RHI/RHISwapchain.h"
#include <vector>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Rev
{

struct FVkBackImage
{
	VkImage Image;
	VkImageView ImageView;
	VmaAllocation Allocation;
	VkExtent3D Extent;
	VkFormat Format;
};

class FVkContext;
class FVkDevice;
class FVkSwapchain : public FRHISwapchain
{
public:
	void CreateSwapchain(const FVkContext* InContext, const FVkDevice* InDevice);
	void Cleanup(const FVkContext* InContext, const FVkDevice* InDevice);

	const VkSwapchainKHR& GetSwapchain() const { return mSwapchain; }
	const VkExtent2D& GetExtent() const { return mExtent; }
	const VkFormat& GetFormat() const { return mFormat; }
	const std::vector<VkImage>& GetImages() const { return mImages; }
	const std::vector<VkImageView>& GetImageViews() const { return mImageViews; }
	const FVkBackImage& GetBackImage() const { return mBackImage; }

private:
	static VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& InAvailableFormats);
	static VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& InAvailablePresentModes);
	static VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& InCapabilities);
	void CreateImageViews(const FVkDevice* InDevice);
	void CreateBackImage(const FVkContext* InContext, const FVkDevice* InDevice);

private:
	VkSwapchainKHR mSwapchain;
	VkExtent2D mExtent;
	VkFormat mFormat;
	std::vector<VkImage> mImages;
	std::vector<VkImageView> mImageViews;
	//back image
	FVkBackImage mBackImage;
};

}