#pragma once
#include "Rev/Render/RHI/RHITexture.h"
#include "VulkanPixelFormat.h"
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Rev
{
class FVulkanSampler;
class FVulkanTexture : public FRHITexture
{
public:
	virtual ~FVulkanTexture() = default;
	virtual const void* GetNativeHandle() const override { return mImage; }
	virtual const FRHISampler* GetSampler() const override;
	virtual void ClearMipData(uint8 InMipLevel) override;

	void Transition(VkImageLayout DstLayout, VkCommandBuffer InCmdBuffer = VK_NULL_HANDLE);
	void Transition(VkImageLayout SrcLayout, VkImageLayout DstLayout, VkCommandBuffer InCmdBuffer = VK_NULL_HANDLE);

	VkImage GetImage() const { return mImage; } //For easy to understand
	VkImageView GetImageView() const { return mImageView; }
	VkClearValue GetClearValue();

protected:
	FVulkanTexture(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc);
	VkExtent3D GetExtent();
	VkExtent2D CalculateMipSize2D(uint32 InMipLevel);
	VkExtent3D CalculateMipSize3D(uint32 InMipLevel);
protected:
	VkImage mImage = VK_NULL_HANDLE;
	VkImageView mImageView = VK_NULL_HANDLE;
	VkImageLayout mImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	VkImageAspectFlags mImageAspectFlags = VK_IMAGE_ASPECT_NONE;
	VmaAllocation mAllocation = VK_NULL_HANDLE;
	Ref<FVulkanSampler> mSampler = nullptr;
};

Ref<FVulkanTexture> CreateVulkanTexture(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc);

}