#include "VulkanTexture.h"
#include "VulkanUtils.h"
#include "Rev/Core/Assert.h"

#include "VulkanDynamicRHI.h"
#include "VulkanTexture2D.h"
#include "VulkanTexture2DArray.h"
#include "VulkanTextureCube.h"
#include "VulkanTextureCubeArray.h"
#include "VulkanTexture3D.h"

namespace Rev
{

FVulkanTexture::~FVulkanTexture()
{
}

FVulkanTexture::FVulkanTexture(const FRHITextureDesc& InDesc)
    : FRHITexture(InDesc)
{
	if (FPixelFormatInfo::HasDepth(InDesc.Format))
		ImageAspectFlags |= VK_IMAGE_ASPECT_DEPTH_BIT;
	if (FPixelFormatInfo::HasStencil(InDesc.Format))
		ImageAspectFlags |= VK_IMAGE_ASPECT_STENCIL_BIT;
	if (ImageAspectFlags == VK_IMAGE_ASPECT_NONE)
		ImageAspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
	SubresourceLayouts.resize(size_t(InDesc.NumMips) * InDesc.GetPhysicalLayerCount(), VK_IMAGE_LAYOUT_UNDEFINED);
}

VkClearValue FVulkanTexture::GetClearValue() const
{
	bool bColorImage = ImageAspectFlags & VK_IMAGE_ASPECT_COLOR_BIT;
	VkClearValue ClearValue;
	if (bColorImage)
	{
		/*for (size_t i = 0; i < 4; i++)
		{
			ClearValue.color.float32[i] = mDesc.ClearColor.RGBA[i];
			switch (GPixelFormats[mDesc.Format].NumComponents)
			{
			case 1:
				ClearValue.color.int32[i] = int32(mDesc.ClearColor.RGBA.R * float(0x7F));
				ClearValue.color.uint32[i] = uint32(mDesc.ClearColor.RGBA.R * float(0xFF));
				break;
			case 2:
				ClearValue.color.int32[i] = int32(mDesc.ClearColor.RGBA.R * float(0x7FFF));
				ClearValue.color.uint32[i] = uint32(mDesc.ClearColor.RGBA.R * float(0xFFFF));
				break;
			case 4:
				ClearValue.color.int32[i] = int32(double(mDesc.ClearColor.RGBA.R) * double(0x7FFFFFFF));
				ClearValue.color.uint32[i] = uint32(mDesc.ClearColor.RGBA.R * double(0xFFFFFFFF));
				break;;
			}
		}*/
		memcpy(&(ClearValue.color.float32), &(TextureDesc.ClearColor.RGBA), 4 * sizeof(float));
	}
	else
	{
		ClearValue.depthStencil.depth = TextureDesc.ClearColor.Depth;
		ClearValue.depthStencil.stencil = TextureDesc.ClearColor.Stencil;
	}

	return ClearValue;
}

VkExtent3D FVulkanTexture::GetExtent() const
{
	uint32 Depth = TextureDesc.Depth <= 0 ? 1 : TextureDesc.Depth;
	return { TextureDesc.Width, TextureDesc.Height, Depth };
}

VkSampleCountFlagBits FVulkanTexture::GetSamplerCount() const
{
	return (VkSampleCountFlagBits)TextureDesc.NumSamples;
}

VkFormat FVulkanTexture::GetPlatformFormat() const
{
	return PlatformFormat;
}

void FVulkanTexture::Release()
{
	REV_CORE_ASSERT(FVulkanDynamicRHI::GetDevice());
	REV_CORE_ASSERT(FVulkanDynamicRHI::GetAllocator());

	for (const auto& Pair : ImageViewCache)
		vkDestroyImageView(FVulkanDynamicRHI::GetDevice(), Pair.second, nullptr);
	ImageViewCache.clear();
	vkDestroyImageView(FVulkanDynamicRHI::GetDevice(), ImageView, nullptr);
	vmaDestroyImage(FVulkanDynamicRHI::GetAllocator(), Image, Allocation);
	Image = VK_NULL_HANDLE;
	ImageView = VK_NULL_HANDLE;
	//ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	Allocation = VK_NULL_HANDLE;
}

VkImageView FVulkanTexture::GetImageView(const FRHITextureSubresourceRange& InRange)
{
	return GetImageView({ InRange, ERHITextureViewType::Auto });
}

VkImageView FVulkanTexture::GetImageView(const FRHITextureViewDesc& InDesc)
{
	const FRHITextureSubresourceRange& InRange = InDesc.Range;
	const uint32 NumMips = InRange.NumMips ? InRange.NumMips : TextureDesc.NumMips - InRange.BaseMip;
	const uint32 TotalLayers = TextureDesc.GetPhysicalLayerCount();
	const uint32 NumLayers = InRange.NumLayers ? InRange.NumLayers : TotalLayers - InRange.BaseLayer;
	REV_CORE_ASSERT(InRange.BaseMip < TextureDesc.NumMips && InRange.BaseMip + NumMips <= TextureDesc.NumMips);
	REV_CORE_ASSERT(InRange.BaseLayer < TotalLayers && InRange.BaseLayer + NumLayers <= TotalLayers);
	VkImageAspectFlags Aspect = ImageAspectFlags;
	if (InRange.Aspect != ERHITextureAspect::Auto)
	{
		Aspect = 0;
		if (EnumHasAnyFlags(InRange.Aspect, ERHITextureAspect::Color)) Aspect |= VK_IMAGE_ASPECT_COLOR_BIT;
		if (EnumHasAnyFlags(InRange.Aspect, ERHITextureAspect::Depth)) Aspect |= VK_IMAGE_ASPECT_DEPTH_BIT;
		if (EnumHasAnyFlags(InRange.Aspect, ERHITextureAspect::Stencil)) Aspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}
	if (InDesc.Type == ERHITextureViewType::ShaderResource && (Aspect & VK_IMAGE_ASPECT_DEPTH_BIT) != 0)
		Aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
	if (InDesc.Type != ERHITextureViewType::Attachment && InRange.BaseMip == 0 && NumMips == TextureDesc.NumMips && InRange.BaseLayer == 0 && NumLayers == TotalLayers && Aspect == ImageAspectFlags)
		return ImageView;

	const uint64 Key = uint64(Aspect) | (uint64(InRange.BaseMip) << 8) | (uint64(NumMips) << 16) |
		(uint64(InRange.BaseLayer) << 24) | (uint64(NumLayers) << 40) | (uint64(InDesc.Type) << 56);
	if (auto It = ImageViewCache.find(Key); It != ImageViewCache.end()) return It->second;

	VkImageViewCreateInfo Info{};
	Info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	Info.image = Image;
	Info.format = PlatformFormat;
	if (TextureDesc.Dimension == ETextureDimension::Texture3D)
		Info.viewType = VK_IMAGE_VIEW_TYPE_3D;
	else if (InDesc.Type == ERHITextureViewType::ShaderResource && TextureDesc.Dimension == ETextureDimension::TextureCube && NumLayers == 6)
		Info.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
	else if (InDesc.Type == ERHITextureViewType::ShaderResource && TextureDesc.Dimension == ETextureDimension::TextureCubeArray && NumLayers % 6 == 0)
		Info.viewType = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
	else
		Info.viewType = NumLayers == 1 ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_2D_ARRAY;
	Info.subresourceRange = { Aspect, InRange.BaseMip, NumMips, InRange.BaseLayer, NumLayers };
	VkImageView View = VK_NULL_HANDLE;
	REV_VK_CHECK(vkCreateImageView(FVulkanDynamicRHI::GetDevice(), &Info, nullptr, &View));
	ImageViewCache.emplace(Key, View);
	return View;
}

VkExtent2D FVulkanTexture::CalculateMipSize2D(uint32 InMipLevel)
{
    return { std::max<uint32>(1, GetWidth() >> InMipLevel), std::max<uint32>(1, GetHeight() >> InMipLevel) };
}

VkExtent3D FVulkanTexture::CalculateMipSize3D(uint32 InMipLevel)
{
    return { std::max<uint32>(1, GetWidth() >> InMipLevel), std::max<uint32>(1, GetHeight() >> InMipLevel), std::max<uint32>(1, GetDepth() >> InMipLevel) };
}

VkImageUsageFlags FVulkanTexture::TranslateImageUsageFlags(ETextureCreateFlags InFlags)
{
	VkImageUsageFlags OutFlags = 0;
	if (EnumHasAnyFlags(InFlags, ETextureCreateFlags::ShaderResource)) OutFlags |= VK_IMAGE_USAGE_SAMPLED_BIT;
	if (EnumHasAnyFlags(InFlags, ETextureCreateFlags::TransferSrc)) OutFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	if (EnumHasAnyFlags(InFlags, ETextureCreateFlags::TransferDst)) OutFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	if (EnumHasAnyFlags(InFlags, ETextureCreateFlags::ColorTarget | ETextureCreateFlags::ColorResolveTarget))
	{
		OutFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		if (EnumHasAnyFlags(InFlags, ETextureCreateFlags::ColorResolveTarget))
			OutFlags |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
	}
	if (EnumHasAnyFlags(InFlags, ETextureCreateFlags::DepthStencilTarget | ETextureCreateFlags::DepthStencilResolveTarget))
	{
		OutFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		/*if (EnumHasAnyFlags(InFlags, ETextureCreateFlags::DepthStencilResolveTarget))
			OutFlags |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;*/
	}
	// Upload/clear are part of the public texture API.
	OutFlags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

	return OutFlags;
}


void FVulkanTexture::DoTransition(VkCommandBuffer InCmdBuffer, VkImageLayout TargetLayout)
{
	if(ImageLayout == TargetLayout)
		return;

	REV_CORE_ASSERT(InCmdBuffer);
	FVulkanUtils::TransitionImage(InCmdBuffer, Image, ImageLayout, TargetLayout, ImageAspectFlags);
	ImageLayout = TargetLayout;
	std::fill(SubresourceLayouts.begin(), SubresourceLayouts.end(), TargetLayout);
}

VkImageLayout FVulkanTexture::GetSubresourceLayout(uint8 InMipLevel, uint16 InArrayIndex) const
{
	const size_t Index = size_t(InArrayIndex) * TextureDesc.NumMips + InMipLevel;
	REV_CORE_ASSERT(Index < SubresourceLayouts.size());
	return SubresourceLayouts[Index];
}

void FVulkanTexture::SetSubresourceLayout(uint8 InMipLevel, uint16 InArrayIndex, VkImageLayout InLayout)
{
	const size_t Index = size_t(InArrayIndex) * TextureDesc.NumMips + InMipLevel;
	REV_CORE_ASSERT(Index < SubresourceLayouts.size());
	SubresourceLayouts[Index] = InLayout;
}

void FVulkanTexture::UploadContent(FVulkanContext* Context, const void* InContent, uint32 InSize, VkExtent3D InExtent, uint8 InMipLevel, uint16 InArrayIndex)
{
	FVulkanUtils::ImmediateUploadImage(Context, Image, ImageAspectFlags, InExtent, InContent, InSize, InMipLevel, InArrayIndex, 1, GetSubresourceLayout(InMipLevel, InArrayIndex));
	SetSubresourceLayout(InMipLevel, InArrayIndex, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

void FVulkanTexture::ClearContent(FVulkanContext* Context, uint8 InMipLevel, uint8 InMipCount, uint16 InArrayIndex, uint16 InArrayCount)
{
	REV_CORE_ASSERT(InMipLevel < TextureDesc.NumMips, "MipLevel out of range");
	const uint8 MipCount = InMipCount ? InMipCount : TextureDesc.NumMips - InMipLevel;
	const uint16 LayerCount = InArrayCount ? InArrayCount : TextureDesc.GetPhysicalLayerCount() - InArrayIndex;
	REV_CORE_ASSERT(InMipLevel + MipCount <= TextureDesc.NumMips && InArrayIndex + LayerCount <= TextureDesc.GetPhysicalLayerCount());
	for (uint8 Mip = InMipLevel; Mip < InMipLevel + MipCount; ++Mip)
		for (uint16 Layer = InArrayIndex; Layer < InArrayIndex + LayerCount; ++Layer)
		{
			FVulkanUtils::ImmediateClearImage(Context, Image, ImageAspectFlags, GetClearValue(), Mip, 1, Layer, 1, GetSubresourceLayout(Mip, Layer));
			SetSubresourceLayout(Mip, Layer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}
	ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

void FVulkanTexture::Resize(uint32 InWidth, uint32 InHeight, uint32 InDepth)
{
	if (InWidth != TextureDesc.Width || InHeight != TextureDesc.Height || InDepth != TextureDesc.Depth)
	{
		Release();
		TextureDesc.Width = InWidth;
		TextureDesc.Height = InHeight;
		TextureDesc.Depth = InDepth;
		ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		std::fill(SubresourceLayouts.begin(), SubresourceLayouts.end(), VK_IMAGE_LAYOUT_UNDEFINED);
		Init();
	}
}

//DynamicRHI
Ref<FRHITexture> FVulkanDynamicRHI::RHICreateTexture(const FRHITextureDesc& InDesc)
{
	REV_CORE_ASSERT(InDesc.Validate(), "Invalid texture descriptor");
	REV_CORE_ASSERT(InDesc.Format < PF_Count && GPixelFormats[InDesc.Format].Supported, "Unsupported texture format");
	REV_CORE_ASSERT(GPixelFormats[InDesc.Format].PlatformFormat != VK_FORMAT_UNDEFINED, "Texture format has no Vulkan mapping");
	VkFormat PlatformFormat = VkFormat(GPixelFormats[InDesc.Format].PlatformFormat);
	if (EnumHasAllFlags(InDesc.Flags, ETextureCreateFlags::SRGB))
		PlatformFormat = FVulkanPixelFormat::GetPlatformFormatSRGB(PlatformFormat);
	VkImageType ImageType = InDesc.Dimension == ETextureDimension::Texture3D ? VK_IMAGE_TYPE_3D : VK_IMAGE_TYPE_2D;
	VkImageCreateFlags ImageFlags = (InDesc.Dimension == ETextureDimension::TextureCube || InDesc.Dimension == ETextureDimension::TextureCubeArray) ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;
	if (EnumHasAllFlags(InDesc.Flags, ETextureCreateFlags::SRGB)) ImageFlags |= VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT;
	VkImageUsageFlags Usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	if (EnumHasAnyFlags(InDesc.Flags, ETextureCreateFlags::ShaderResource)) Usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
	if (EnumHasAnyFlags(InDesc.Flags, ETextureCreateFlags::TransferSrc)) Usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	if (EnumHasAnyFlags(InDesc.Flags, ETextureCreateFlags::ColorTarget | ETextureCreateFlags::ColorResolveTarget)) Usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	if (EnumHasAnyFlags(InDesc.Flags, ETextureCreateFlags::DepthStencilTarget | ETextureCreateFlags::DepthStencilResolveTarget)) Usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	VkImageFormatProperties Properties{};
	const VkResult CapabilityResult = vkGetPhysicalDeviceImageFormatProperties(GetPhysicalDevice(), PlatformFormat, ImageType, VK_IMAGE_TILING_OPTIMAL, Usage, ImageFlags, &Properties);
	REV_CORE_ASSERT(CapabilityResult == VK_SUCCESS, "Texture descriptor is not supported by this Vulkan device");
	REV_CORE_ASSERT(InDesc.Width <= Properties.maxExtent.width && InDesc.Height <= Properties.maxExtent.height && InDesc.Depth <= Properties.maxExtent.depth);
	REV_CORE_ASSERT(InDesc.NumMips <= Properties.maxMipLevels && InDesc.GetPhysicalLayerCount() <= Properties.maxArrayLayers);
	REV_CORE_ASSERT((Properties.sampleCounts & VkSampleCountFlagBits(InDesc.NumSamples)) != 0, "Texture sample count is not supported by this format");
	switch (InDesc.Dimension)
	{
	case ETextureDimension::Texture2D:
		return CreateRef<FVulkanTexture2D>(InDesc);
	case ETextureDimension::Texture2DArray:
		return CreateRef<FVulkanTexture2DArray>(InDesc);
	case ETextureDimension::TextureCube:
		return CreateRef<FVulkanTextureCube>(InDesc);
	case ETextureDimension::TextureCubeArray:
		return CreateRef<FVulkanTextureCubeArray>(InDesc);
	case ETextureDimension::Texture3D:
		return CreateRef<FVulkanTexture3D>(InDesc);
	default:
		break;
	}
	REV_CORE_ASSERT(false, "Unsupported Texture Dimension!");
	return nullptr;
}

void FVulkanDynamicRHI::RHIResizeTexture(FRHITexture* InTexture, uint32 InWidth, uint32 InHeight, uint32 InDepth)
{
	FVulkanTexture* Texture = FVulkanTexture::Cast(InTexture);
	if (Texture)
	{
		Texture->Resize(InWidth, InHeight, InDepth);
	}
}

}


