#include "VulkanTexture3D.h"
#include "VulkanPixelFormat.h"
#include "VulkanUtils.h"
#include "VulkanDynamicRHI.h"
#include "Core/VulkanEnum.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FVulkanTexture3D::FVulkanTexture3D(const FRHITextureDesc& InDesc)
    : FVulkanTexture(InDesc)
{
    REV_CORE_ASSERT(InDesc.Dimension == ETextureDimension::Texture3D);
    REV_CORE_ASSERT(InDesc.ArraySize == 1, "3D textures do not support arrays.");
    REV_CORE_ASSERT(InDesc.NumSamples == 1, "3D textures do not support multisampling.");
    Init();
}

FVulkanTexture3D::~FVulkanTexture3D()
{
    Release();
}

void FVulkanTexture3D::UpdateContent(FVulkanContext* Context, const void* InContent, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex)
{
    REV_CORE_ASSERT(InArrayIndex == 0, "ArrayIndex must be 0 for 3D texture");
    REV_CORE_ASSERT(InMipLevel < TextureDesc.NumMips, "MipLevel out of range");

    VkExtent3D MipSize = CalculateMipSize3D(InMipLevel);
    uint32 BlockBytes = GPixelFormats[TextureDesc.Format].BlockBytes;
    REV_CORE_ASSERT(InSize == MipSize.width * MipSize.height * MipSize.depth * BlockBytes, "Data size mismatch");

    FVulkanUtils::ImmediateUploadImage(Context, Image, ImageAspectFlags, MipSize, InContent, InSize, InMipLevel, InArrayIndex);
}

void FVulkanTexture3D::Init()
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
    ImageCreateInfo.imageType = VK_IMAGE_TYPE_3D;
    ImageCreateInfo.format = ImageFormat;
    ImageCreateInfo.extent = GetExtent();
    ImageCreateInfo.mipLevels = TextureDesc.NumMips;
    ImageCreateInfo.arrayLayers = 1;
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
    ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_3D;
    ImageViewCreateInfo.image = Image;
    ImageViewCreateInfo.format = ImageFormat;
    ImageViewCreateInfo.subresourceRange.aspectMask = ImageAspectFlags;
    ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    ImageViewCreateInfo.subresourceRange.levelCount = TextureDesc.NumMips;
    ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    ImageViewCreateInfo.subresourceRange.layerCount = 1;

    REV_VK_CHECK(vkCreateImageView(FVulkanDynamicRHI::GetDevice(), &ImageViewCreateInfo, nullptr, &ImageView));
}

}
