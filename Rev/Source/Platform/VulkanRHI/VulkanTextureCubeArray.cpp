#include "VulkanTextureCubeArray.h"
#include "VulkanPixelFormat.h"
#include "VulkanUtils.h"
#include "VulkanDynamicRHI.h"
#include "Core/VulkanEnum.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FVulkanTextureCubeArray::FVulkanTextureCubeArray(const FRHITextureDesc& InDesc)
    : FVulkanTexture(InDesc)
{
    REV_CORE_ASSERT(InDesc.Dimension == ETextureDimension::TextureCubeArray);
    REV_CORE_ASSERT(InDesc.Depth == 1);
    REV_CORE_ASSERT(InDesc.NumSamples == 1, "Cube array does not support multisampling.");
    Init();
}

FVulkanTextureCubeArray::~FVulkanTextureCubeArray()
{
    Release();
}

void FVulkanTextureCubeArray::UpdateContent(FVulkanContext* Context, const void* InContent, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex)
{
    uint32 TotalLayers = TextureDesc.ArraySize * 6;
    REV_CORE_ASSERT(InArrayIndex < TotalLayers, "ArrayIndex out of range for cube array texture");
    REV_CORE_ASSERT(InMipLevel < TextureDesc.NumMips, "MipLevel out of range");

    VkExtent2D MipSize = CalculateMipSize2D(InMipLevel);
    REV_CORE_ASSERT(InSize == MipSize.width * MipSize.height * GPixelFormats[TextureDesc.Format].BlockBytes, "Data size mismatch");

    FVulkanUtils::ImmediateUploadImage(Context, Image, ImageAspectFlags, { MipSize.width, MipSize.height, 1 }, InContent, InSize, InMipLevel, InArrayIndex);
}

void FVulkanTextureCubeArray::Init()
{
    REV_CORE_ASSERT(FVulkanDynamicRHI::GetDevice());
    REV_CORE_ASSERT(FVulkanDynamicRHI::GetAllocator());

    VkFormat ImageFormat = (VkFormat)GPixelFormats[TextureDesc.Format].PlatformFormat;
    VkImageCreateFlags ImageFlags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    if (EnumHasAllFlags(TextureDesc.Flags, ETextureCreateFlags::SRGB))
    {
        ImageFormat = FVulkanPixelFormat::GetPlatformFormatSRGB(ImageFormat);
        ImageFlags |= VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
    }

    uint32 TotalLayers = TextureDesc.ArraySize * 6;

    VkImageCreateInfo ImageCreateInfo{};
    ImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    ImageCreateInfo.pNext = nullptr;
    ImageCreateInfo.flags = ImageFlags;
    ImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    ImageCreateInfo.format = ImageFormat;
    ImageCreateInfo.extent = GetExtent();
    ImageCreateInfo.mipLevels = TextureDesc.NumMips;
    ImageCreateInfo.arrayLayers = TotalLayers;
    ImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    ImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    ImageCreateInfo.usage = TranslateImageUsageFlags(TextureDesc.Flags);
    ImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    ImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VmaAllocationCreateInfo ImageAllocinfo = {};
    ImageAllocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    ImageAllocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    REV_VK_CHECK(vmaCreateImage(FVulkanDynamicRHI::GetAllocator(), &ImageCreateInfo, &ImageAllocinfo, &Image, &Allocation, nullptr));

    VkImageViewCreateInfo ImageViewCreateInfo{};
    ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
    ImageViewCreateInfo.image = Image;
    ImageViewCreateInfo.format = ImageFormat;
    ImageViewCreateInfo.subresourceRange.aspectMask = ImageAspectFlags;
    ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    ImageViewCreateInfo.subresourceRange.levelCount = TextureDesc.NumMips;
    ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    ImageViewCreateInfo.subresourceRange.layerCount = TotalLayers;

    REV_VK_CHECK(vkCreateImageView(FVulkanDynamicRHI::GetDevice(), &ImageViewCreateInfo, nullptr, &ImageView));
}

}
