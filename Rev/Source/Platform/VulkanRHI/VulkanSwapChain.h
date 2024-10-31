#pragma once
#include "Rev/Core/Base.h"
#include <vector>
#include <vulkan/vulkan.h> 

namespace Rev
{
class FVulkanTextureSwapchain;
class FVulkanSwapchain
{
public:
	void CreateSwapchain(VkPresentModeKHR InPresentMode);
	void Cleanup();
	void NextFrame(uint64_t timeout,VkSemaphore semaphore, VkFence fence);

	const VkSwapchainKHR& GetSwapchain() const { return mSwapchain; }
	const VkExtent2D& GetExtent() const { return mExtent; }
	const VkFormat& GetFormat() const { return mFormat; }
	const FVulkanTextureSwapchain* GetTexture(uint32 Index) const;
	const FVulkanTextureSwapchain* GetCurrentTexture() const { return GetTexture(mCurrentTextureIndex); }
	VkImage GetCurrentImage() const;
	VkImageView GetCurrentImageView() const;
	const uint32 GetCurrentTextureIndex() const { return mCurrentTextureIndex; }

private:
	static VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& InAvailableFormats);
	static VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& InAvailablePresentModes, VkPresentModeKHR InTargetPresentMode);
	static VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& InCapabilities);

private:
	VkSwapchainKHR mSwapchain;
	VkExtent2D mExtent;
	VkFormat mFormat;
	std::vector<Ref<FVulkanTextureSwapchain>> mTextures;
	uint32 mCurrentTextureIndex = 0;
};

}