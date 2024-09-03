#pragma once
#include "Rev/Render/RHI/RHITexture.h"
#include "VkPixelFormat.h"
#include <Vulkan/vulkan.h>
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

	const FVulkanFormatInfo& GetFormatInfo() const { return mFormatInfo; }
	void Transition(VkImageLayout DstLayout, VkCommandBuffer InCmdBuffer = VK_NULL_HANDLE);
	void Transition(VkImageLayout SrcLayout, VkImageLayout DstLayout, VkCommandBuffer InCmdBuffer = VK_NULL_HANDLE);

protected:
	FVulkanTexture(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc);
	VkExtent3D GetExtent();
	VkExtent2D CalculateMipSize2D(uint32 InMipLevel);
	VkExtent3D CalculateMipSize3D(uint32 InMipLevel);
	VkClearValue GetClearValue();
protected:
	FVulkanFormatInfo mFormatInfo;
	VkImage mImage = VK_NULL_HANDLE;
	VkImageView mImageView = VK_NULL_HANDLE;
	VkImageLayout mImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	VmaAllocation mAllocation = VK_NULL_HANDLE;
	Ref<FVulkanSampler> mSampler = nullptr;
};

Ref<FVulkanTexture> CreateVkTexture(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc);

}