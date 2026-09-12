#include "VulkanTextureCubeArray.h"
#include "VulkanDynamicRHI.h"
#include "VulkanPixelFormat.h"
#include "VulkanUtils.h"
#include "Rev/Core/Assert.h"

namespace Rev
{
FVulkanTextureCubeArray::FVulkanTextureCubeArray(const FRHITextureDesc& InDesc) : FVulkanTexture(InDesc)
{
	REV_CORE_ASSERT(InDesc.Dimension == ETextureDimension::TextureCubeArray && InDesc.Depth == 1 && InDesc.NumSamples == 1);
	Init();
}
FVulkanTextureCubeArray::~FVulkanTextureCubeArray() { Release(); }

void FVulkanTextureCubeArray::UpdateContent(FVulkanContext* Context, const void* InContent, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex)
{
	REV_CORE_ASSERT(InArrayIndex < TextureDesc.GetPhysicalLayerCount() && InMipLevel < TextureDesc.NumMips);
	const VkExtent2D MipSize = CalculateMipSize2D(InMipLevel);
	REV_CORE_ASSERT(InSize == MipSize.width * MipSize.height * GPixelFormats[TextureDesc.Format].BlockBytes);
	UploadContent(Context, InContent, InSize, { MipSize.width, MipSize.height, 1 }, InMipLevel, InArrayIndex);
}

void FVulkanTextureCubeArray::Init()
{
	PlatformFormat = (VkFormat)GPixelFormats[TextureDesc.Format].PlatformFormat;
	VkImageCreateFlags Flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
	if (EnumHasAllFlags(TextureDesc.Flags, ETextureCreateFlags::SRGB)) { PlatformFormat = FVulkanPixelFormat::GetPlatformFormatSRGB(PlatformFormat); Flags |= VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT; }
	VkImageCreateInfo Info{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	Info.flags = Flags; Info.imageType = VK_IMAGE_TYPE_2D; Info.format = PlatformFormat; Info.extent = GetExtent();
	Info.mipLevels = TextureDesc.NumMips; Info.arrayLayers = TextureDesc.GetPhysicalLayerCount(); Info.samples = VK_SAMPLE_COUNT_1_BIT;
	Info.tiling = VK_IMAGE_TILING_OPTIMAL; Info.usage = TranslateImageUsageFlags(TextureDesc.Flags); Info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	VmaAllocationCreateInfo AllocationInfo{}; AllocationInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY; AllocationInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	REV_VK_CHECK(vmaCreateImage(FVulkanDynamicRHI::GetAllocator(), &Info, &AllocationInfo, &Image, &Allocation, nullptr));
	VkImageViewCreateInfo View{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	View.image = Image; View.format = PlatformFormat; View.viewType = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
	View.subresourceRange = { ImageAspectFlags, 0, TextureDesc.NumMips, 0, TextureDesc.GetPhysicalLayerCount() };
	REV_VK_CHECK(vkCreateImageView(FVulkanDynamicRHI::GetDevice(), &View, nullptr, &ImageView));
}
}
