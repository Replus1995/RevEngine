#include "VulkanTexture2D.h"
#include "VulkanCore.h"
#include "VulkanUtils.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FVulkanTexture2D::FVulkanTexture2D(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc)
	: FVulkanTexture(InDesc, InSamplerDesc)
{
	REV_CORE_ASSERT(InDesc.Dimension == ETextureDimension::Texture2D);
	CreateResource();
}

FVulkanTexture2D::~FVulkanTexture2D()
{
    REV_CORE_ASSERT(FVulkanCore::GetDevice());
    REV_CORE_ASSERT(FVulkanCore::GetAllocator());

    vkDestroyImageView(FVulkanCore::GetDevice(), mImageView, nullptr);
    vmaDestroyImage(FVulkanCore::GetAllocator(), mImage, mAllocation);
}

void FVulkanTexture2D::UpdateLayerData(const void* InData, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex, int32 InDepth)
{
    if (mDesc.NumSamples != 1)
    {
        REV_CORE_ERROR("Updating multisample texture is not allowed");
        return;
    }
    REV_CORE_ASSERT(InDepth <= 0, "Depth must be 0(-1) for 2d texture");
    REV_CORE_ASSERT(InArrayIndex == 0, "ArrayIndex must be 0 for 2d texture");
    REV_CORE_ASSERT(InMipLevel < mDesc.NumMips, "MipLevel out of range");

    VkExtent2D MipSize = CalculateMipSize2D(InMipLevel);
    REV_CORE_ASSERT(InSize == MipSize.width * MipSize.height * GPixelFormats[mDesc.Format].BlockBytes, "Data size mismatch");

    FVulkanUtils::ImmediateUploadImage(mImage, mImageAspectFlags, { MipSize.width, MipSize.height, 1 }, InData, InSize, InMipLevel, 0, 0);
}

void FVulkanTexture2D::ClearLayerData(uint8 InMipLevel, uint16 InArrayIndex, int32 InDepth)
{
    REV_CORE_ASSERT(InDepth <= 0, "Depth must be 0(-1) for 2d texture");
    REV_CORE_ASSERT(InArrayIndex == 0, "ArrayIndex must be 0 for 2d texture");
    REV_CORE_ASSERT(InMipLevel < mDesc.NumMips, "MipLevel out of range");

    FVulkanUtils::ImmediateClearImage(mImage, mImageAspectFlags, GetClearValue(), InMipLevel, 1, 0, 0);
}

void FVulkanTexture2D::CreateResource()
{
    REV_CORE_ASSERT(FVulkanCore::GetDevice());
    REV_CORE_ASSERT(FVulkanCore::GetAllocator());

    VkFormat ImageFormat = (VkFormat)GPixelFormats[mDesc.Format].PlatformFormat;

    VkImageCreateInfo ImageCreateInfo{};
    ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    ImageCreateInfo.extent = GetExtent();
    ImageCreateInfo.mipLevels = mDesc.NumMips;
    ImageCreateInfo.arrayLayers = mDesc.ArraySize;
    ImageCreateInfo.format = ImageFormat;
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
    REV_VK_CHECK(vmaCreateImage(FVulkanCore::GetAllocator(), &ImageCreateInfo, &ImageAllocinfo, &mImage, &mAllocation, nullptr));

    //build a image-view for the draw image to use for rendering
    VkImageViewCreateInfo ImageViewCreateInfo{}; // = FVkInit::ImageViewCreateInfo2D(mFormatInfo.Format, mImage, VK_IMAGE_ASPECT_COLOR_BIT);
    ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    ImageViewCreateInfo.image = mImage;
    ImageViewCreateInfo.format = ImageFormat;
    ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    ImageViewCreateInfo.subresourceRange.levelCount = 1;
    ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    ImageViewCreateInfo.subresourceRange.layerCount = 1;
    ImageViewCreateInfo.subresourceRange.aspectMask = mImageAspectFlags;

    REV_VK_CHECK(vkCreateImageView(FVulkanCore::GetDevice(), &ImageViewCreateInfo, nullptr, &mImageView));
}

}