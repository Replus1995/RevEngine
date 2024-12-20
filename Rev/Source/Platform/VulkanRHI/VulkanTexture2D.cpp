#include "VulkanTexture2D.h"
#include "VulkanPixelFormat.h"
#include "VulkanUtils.h"
#include "VulkanDynamicRHI.h"
#include "Core/VulkanEnum.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FVulkanTexture2D::FVulkanTexture2D(const FRHITextureDesc& InDesc)
	: FVulkanTexture(InDesc)
{
	REV_CORE_ASSERT(InDesc.Dimension == ETextureDimension::Texture2D);
    REV_CORE_ASSERT(InDesc.Depth == 1);
    REV_CORE_ASSERT(InDesc.ArraySize == 1);
    Init();
}

FVulkanTexture2D::~FVulkanTexture2D()
{
    Release();
}

void FVulkanTexture2D::UpdateContent(FVulkanContext* Context, const void* InContent, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex)
{
    if (TextureDesc.NumSamples != 1)
    {
        REV_CORE_ERROR("Updating multisample texture is not allowed");
        return;
    }
    REV_CORE_ASSERT(InArrayIndex == 0, "ArrayIndex must be 0 for 2d texture");
    REV_CORE_ASSERT(InMipLevel < TextureDesc.NumMips, "MipLevel out of range");

    VkExtent2D MipSize = CalculateMipSize2D(InMipLevel);
    REV_CORE_ASSERT(InSize == MipSize.width * MipSize.height * GPixelFormats[TextureDesc.Format].BlockBytes, "Data size mismatch");

    FVulkanUtils::ImmediateUploadImage(Context, mImage, mImageAspectFlags, { MipSize.width, MipSize.height, 1 }, InContent, InSize, InMipLevel);
}

void FVulkanTexture2D::Init()
{
    REV_CORE_ASSERT(FVulkanDynamicRHI::GetDevice());
    REV_CORE_ASSERT(FVulkanDynamicRHI::GetAllocator());

    VkFormat ImageFormat = (VkFormat)GPixelFormats[TextureDesc.Format].PlatformFormat;
    VkImageCreateFlags ImageFlags = 0;
    if (EnumHasAllFlags(TextureDesc.Flags, ETextureCreateFlags::SRGB))
    {
        ImageFormat = FVulkanPixelFormat::GetPlatformFormatSRGB(ImageFormat);
        ImageFlags |= VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
    }

    VkImageCreateInfo ImageCreateInfo{};
    ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    ImageCreateInfo.pNext = nullptr;
    ImageCreateInfo.flags = ImageFlags;
    ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    ImageCreateInfo.format = ImageFormat;
    ImageCreateInfo.extent = GetExtent();
    ImageCreateInfo.mipLevels = TextureDesc.NumMips;
    ImageCreateInfo.arrayLayers = 1;
    ImageCreateInfo.samples = (VkSampleCountFlagBits)TextureDesc.NumSamples;
    ImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    ImageCreateInfo.usage = TranslateImageUsageFlags(TextureDesc.Flags);
    ImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    ImageCreateInfo.initialLayout = mImageLayout;

    //for the draw image, we want to allocate it from gpu local memory
    VmaAllocationCreateInfo ImageAllocinfo = {};
    ImageAllocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    ImageAllocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    //allocate and create the image
    REV_VK_CHECK(vmaCreateImage(FVulkanDynamicRHI::GetAllocator(), &ImageCreateInfo, &ImageAllocinfo, &mImage, &mAllocation, nullptr));

    //build a image-view for the draw image to use for rendering
    VkImageViewCreateInfo ImageViewCreateInfo{}; // = FVkInit::ImageViewCreateInfo2D(mFormatInfo.Format, mImage, VK_IMAGE_ASPECT_COLOR_BIT);
    ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    ImageViewCreateInfo.image = mImage;
    ImageViewCreateInfo.format = ImageFormat;
    ImageViewCreateInfo.subresourceRange.aspectMask = mImageAspectFlags;
    ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    ImageViewCreateInfo.subresourceRange.levelCount = TextureDesc.NumMips;
    ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    ImageViewCreateInfo.subresourceRange.layerCount = 1;

    REV_VK_CHECK(vkCreateImageView(FVulkanDynamicRHI::GetDevice(), &ImageViewCreateInfo, nullptr, &mImageView));
}

FVulkanTextureSwapchain::FVulkanTextureSwapchain(VkImage InSwapchainImage, VkFormat InFormat, uint16 InWidth, uint16 InHeight)
    : FVulkanTexture(FRHITextureDesc::Create2D(InWidth, InHeight, PF_Unknown))
    , mPlatformFormat(InFormat)
{
    mImage = InSwapchainImage; 
    mImageAspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
    Init();
}

FVulkanTextureSwapchain::~FVulkanTextureSwapchain()
{
    Release();
}

void FVulkanTextureSwapchain::Init()
{
    VkImageViewCreateInfo ImageViewCreateInfo{};
    ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    ImageViewCreateInfo.image = mImage;
    ImageViewCreateInfo.format = mPlatformFormat;
    ImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    ImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    ImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    ImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    ImageViewCreateInfo.subresourceRange.aspectMask = mImageAspectFlags;
    ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    ImageViewCreateInfo.subresourceRange.levelCount = 1;
    ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    ImageViewCreateInfo.subresourceRange.layerCount = 1;

    REV_VK_CHECK(vkCreateImageView(FVulkanDynamicRHI::GetDevice(), &ImageViewCreateInfo, nullptr, &mImageView));
}

void FVulkanTextureSwapchain::Release()
{
    REV_CORE_ASSERT(FVulkanDynamicRHI::GetDevice());

    vkDestroyImageView(FVulkanDynamicRHI::GetDevice(), mImageView, nullptr);
    mImage = VK_NULL_HANDLE;
    mImageView = VK_NULL_HANDLE;
}


}