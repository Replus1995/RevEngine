#include "VkTexture2D.h"
#include "VkContext.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FVkTexture2D::FVkTexture2D(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc)
	: FVkTexture(InDesc, InSamplerDesc)
{
	REV_CORE_ASSERT(InDesc.Dimension == ETextureDimension::Texture2D);
	CreateResource();
}

FVkTexture2D::~FVkTexture2D()
{
    REV_CORE_ASSERT(FVkCore::GetDevice());
    REV_CORE_ASSERT(FVkCore::GetAllocator());

    vkDestroyImageView(FVkCore::GetDevice(), mImageView, nullptr);
    vmaDestroyImage(FVkCore::GetAllocator(), mImage, mAllocation);
}

void FVkTexture2D::UpdateLayerData(const void* InData, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex, int32 InDepth)
{
}

void FVkTexture2D::ClearLayerData(uint8 InMipLevel, uint16 InArrayIndex, int32 InDepth)
{
}

void FVkTexture2D::CreateResource()
{
    REV_CORE_ASSERT(FVkCore::GetDevice());
    REV_CORE_ASSERT(FVkCore::GetAllocator());

    VkImageCreateInfo ImageCreateInfo{};
    ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    ImageCreateInfo.extent = GetExtent();
    ImageCreateInfo.mipLevels = mDesc.NumMips;
    ImageCreateInfo.arrayLayers = mDesc.ArraySize;
    ImageCreateInfo.format = mFormatInfo.Format;
    ImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    ImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    ImageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    ImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    ImageCreateInfo.samples = (VkSampleCountFlagBits)mDesc.NumSamples;
    ImageCreateInfo.flags = 0;

    //for the draw image, we want to allocate it from gpu local memory
    VmaAllocationCreateInfo ImageAllocinfo = {};
    ImageAllocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    ImageAllocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    //allocate and create the image
    vmaCreateImage(FVkCore::GetAllocator(), &ImageCreateInfo, &ImageAllocinfo, &mImage, &mAllocation, nullptr);

    //build a image-view for the draw image to use for rendering
    VkImageViewCreateInfo ImageViewCreateInfo{}; // = FVkInit::ImageViewCreateInfo2D(mFormatInfo.Format, mImage, VK_IMAGE_ASPECT_COLOR_BIT);
    ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    ImageViewCreateInfo.image = mImage;
    ImageViewCreateInfo.format = mFormatInfo.Format;
    ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    ImageViewCreateInfo.subresourceRange.levelCount = 1;
    ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    ImageViewCreateInfo.subresourceRange.layerCount = 1;
    ImageViewCreateInfo.subresourceRange.aspectMask = mFormatInfo.AspectFlags;

    REV_VK_CHECK(vkCreateImageView(FVkCore::GetDevice(), &ImageViewCreateInfo, nullptr, &mImageView));
}

}