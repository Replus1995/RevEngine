#include "VulkanTexture2DArray.h"
#include "VulkanPixelFormat.h"
#include "VulkanUtils.h"
#include "VulkanDynamicRHI.h"
#include "Core/VulkanEnum.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FVulkanTexture2DArray::FVulkanTexture2DArray(const FRHITextureDesc& InDesc)
	: FVulkanTexture(InDesc)
{
	REV_CORE_ASSERT(InDesc.Dimension == ETextureDimension::Texture2DArray);
    REV_CORE_ASSERT(InDesc.Depth == 1)
    REV_CORE_ASSERT(InDesc.NumSamples == 1, "Only texture2d can be multisampled.");
    Init();
}

FVulkanTexture2DArray::~FVulkanTexture2DArray()
{
    Release();
}

void FVulkanTexture2DArray::UpdateContent(FVulkanContext* Context, const void* InContent, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex)
{
    REV_CORE_ASSERT(InArrayIndex < TextureDesc.ArraySize, "ArrayIndex out of range");
    REV_CORE_ASSERT(InMipLevel < TextureDesc.NumMips, "MipLevel out of range");

    VkExtent2D MipSize = CalculateMipSize2D(InMipLevel);
    REV_CORE_ASSERT(InSize == MipSize.width * MipSize.height * GPixelFormats[TextureDesc.Format].BlockBytes, "Data size mismatch");

    FVulkanUtils::ImmediateUploadImage(Context, Image, ImageAspectFlags, { MipSize.width, MipSize.height, 1 }, InContent, InSize, InMipLevel, InArrayIndex);
}

void FVulkanTexture2DArray::Init()
{
    REV_CORE_ASSERT(FVulkanDynamicRHI::GetDevice());
    REV_CORE_ASSERT(FVulkanDynamicRHI::GetAllocator());

    PlatformFormat = (VkFormat)GPixelFormats[TextureDesc.Format].PlatformFormat;
    VkImageCreateFlags ImageFlags = 0;
    if (EnumHasAllFlags(TextureDesc.Flags, ETextureCreateFlags::SRGB))
    {
        PlatformFormat = FVulkanPixelFormat::GetPlatformFormatSRGB(PlatformFormat);
        ImageFlags |= VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
    }

    VkImageCreateInfo ImageCreateInfo{};
    ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    ImageCreateInfo.pNext = nullptr;
    ImageCreateInfo.flags = ImageFlags;
    ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    ImageCreateInfo.format = PlatformFormat;
    ImageCreateInfo.extent = GetExtent();
    ImageCreateInfo.mipLevels = TextureDesc.NumMips;
    ImageCreateInfo.arrayLayers = TextureDesc.ArraySize;
    ImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    ImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    ImageCreateInfo.usage = TranslateImageUsageFlags(TextureDesc.Flags);
    ImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    ImageCreateInfo.initialLayout = ImageLayout;

    //for the draw image, we want to allocate it from gpu local memory
    VmaAllocationCreateInfo ImageAllocinfo = {};
    ImageAllocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    ImageAllocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    //allocate and create the image
    REV_VK_CHECK(vmaCreateImage(FVulkanDynamicRHI::GetAllocator(), &ImageCreateInfo, &ImageAllocinfo, &Image, &Allocation, nullptr));

    //build a image-view for the draw image to use for rendering
    VkImageViewCreateInfo ImageViewCreateInfo{}; // = FVkInit::ImageViewCreateInfo2D(mFormatInfo.Format, mImage, VK_IMAGE_ASPECT_COLOR_BIT);
    ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    ImageViewCreateInfo.image = Image;
    ImageViewCreateInfo.format = PlatformFormat;
    ImageViewCreateInfo.subresourceRange.aspectMask = ImageAspectFlags;
    ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    ImageViewCreateInfo.subresourceRange.levelCount = TextureDesc.NumMips;
    ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    ImageViewCreateInfo.subresourceRange.layerCount = TextureDesc.ArraySize;

    REV_VK_CHECK(vkCreateImageView(FVulkanDynamicRHI::GetDevice(), &ImageViewCreateInfo, nullptr, &ImageView));
}

}