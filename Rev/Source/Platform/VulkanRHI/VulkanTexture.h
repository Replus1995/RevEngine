#pragma once
#include "Rev/Render/RHI/RHITexture.h"
#include "VulkanPixelFormat.h"
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Rev
{
class FVulkanContext;
class FVulkanSamplerState;
class FVulkanTexture : public FRHITexture
{
public:
	virtual ~FVulkanTexture();
	virtual const void* GetNativeHandle() const override { return mImage; }

	VkImage GetImage() const { return mImage; } //For easy to understand
	VkImageView GetImageView() const { return mImageView; }
	VkImageAspectFlags GetAspectFlags() const { return mImageAspectFlags; }
	VkImageLayout GetImageLayout() const { return mImageLayout; }
	VkClearValue GetClearValue() const;
	VkExtent3D GetExtent() const;
	VkSampleCountFlagBits GetSamplerCount() const;

	static FVulkanTexture* Cast(FRHITexture* InTexture) { return static_cast<FVulkanTexture*>(InTexture); }

	void DoTransition(VkCommandBuffer InCmdBuffer, VkImageLayout TargetLayout);

	virtual void UpdateContent(FVulkanContext* Context, const void* InContent, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex) = 0;
	void ClearContent(FVulkanContext* Context, uint8 InMipLevel, uint8 InMipCount, uint16 InArrayIndex, uint16 InArrayCount);
	void Resize(uint32 InWidth, uint32 InHeight, uint32 InDepth);

	virtual bool IsSwapchainTexture() const { return false; }

protected:
	FVulkanTexture(const FRHITextureDesc& InDesc);
	virtual void Init() = 0;
	virtual void Release();
	VkExtent2D CalculateMipSize2D(uint32 InMipLevel);
	VkExtent3D CalculateMipSize3D(uint32 InMipLevel);
	static VkImageUsageFlags TranslateImageUsageFlags(ETextureCreateFlags InFlags);


protected:
	VkImage mImage = VK_NULL_HANDLE;
	VkImageView mImageView = VK_NULL_HANDLE;
	VkImageAspectFlags mImageAspectFlags = VK_IMAGE_ASPECT_NONE;
	VkImageLayout mImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	VmaAllocation mAllocation = VK_NULL_HANDLE;
	Ref<FVulkanSamplerState> mSampler = nullptr;
};

}