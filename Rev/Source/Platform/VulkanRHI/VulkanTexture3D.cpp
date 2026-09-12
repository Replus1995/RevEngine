#include "VulkanTexture3D.h"
#include "VulkanDynamicRHI.h"
#include "VulkanPixelFormat.h"
#include "VulkanUtils.h"
#include "Rev/Core/Assert.h"

namespace Rev
{
FVulkanTexture3D::FVulkanTexture3D(const FRHITextureDesc& InDesc) : FVulkanTexture(InDesc)
{
	REV_CORE_ASSERT(InDesc.Dimension == ETextureDimension::Texture3D && InDesc.ArraySize == 1 && InDesc.NumSamples == 1);
	Init();
}
FVulkanTexture3D::~FVulkanTexture3D() { Release(); }

void FVulkanTexture3D::UpdateContent(FVulkanContext* Context, const void* InContent, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex)
{
	REV_CORE_ASSERT(InArrayIndex == 0 && InMipLevel < TextureDesc.NumMips);
	const VkExtent3D MipSize = CalculateMipSize3D(InMipLevel);
	REV_CORE_ASSERT(InSize == MipSize.width * MipSize.height * MipSize.depth * GPixelFormats[TextureDesc.Format].BlockBytes);
	UploadContent(Context, InContent, InSize, MipSize, InMipLevel, 0);
}

void FVulkanTexture3D::Init()
{
	PlatformFormat = (VkFormat)GPixelFormats[TextureDesc.Format].PlatformFormat;
	if (EnumHasAllFlags(TextureDesc.Flags, ETextureCreateFlags::SRGB)) PlatformFormat = FVulkanPixelFormat::GetPlatformFormatSRGB(PlatformFormat);
	VkImageCreateInfo Info{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	Info.imageType = VK_IMAGE_TYPE_3D; Info.format = PlatformFormat; Info.extent = GetExtent(); Info.mipLevels = TextureDesc.NumMips;
	Info.arrayLayers = 1; Info.samples = VK_SAMPLE_COUNT_1_BIT; Info.tiling = VK_IMAGE_TILING_OPTIMAL;
	Info.usage = TranslateImageUsageFlags(TextureDesc.Flags); Info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	VmaAllocationCreateInfo AllocationInfo{}; AllocationInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY; AllocationInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	REV_VK_CHECK(vmaCreateImage(FVulkanDynamicRHI::GetAllocator(), &Info, &AllocationInfo, &Image, &Allocation, nullptr));
	VkImageViewCreateInfo View{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	View.image = Image; View.format = PlatformFormat; View.viewType = VK_IMAGE_VIEW_TYPE_3D;
	View.subresourceRange = { ImageAspectFlags, 0, TextureDesc.NumMips, 0, 1 };
	REV_VK_CHECK(vkCreateImageView(FVulkanDynamicRHI::GetDevice(), &View, nullptr, &ImageView));
}
}
