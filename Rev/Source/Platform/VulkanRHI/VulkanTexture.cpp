#include "VulkanTexture.h"
#include "VulkanUtils.h"
#include "Rev/Core/Assert.h"

#include "VulkanDynamicRHI.h"
#include "VulkanTexture2D.h"
#include "VulkanTexture2DArray.h"
#include "VulkanTextureCube.h"

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

	vkDestroyImageView(FVulkanDynamicRHI::GetDevice(), ImageView, nullptr);
	vmaDestroyImage(FVulkanDynamicRHI::GetAllocator(), Image, Allocation);
	Image = VK_NULL_HANDLE;
	ImageView = VK_NULL_HANDLE;
	//ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	Allocation = VK_NULL_HANDLE;
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
	VkImageUsageFlags OutFlags = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	if (EnumHasAnyFlags(InFlags, ETextureCreateFlags::ColorTarget | ETextureCreateFlags::ColorResolveTarget))
	{
		OutFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		if (EnumHasAnyFlags(InFlags, ETextureCreateFlags::ColorResolveTarget))
			OutFlags |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
	}
	else if (EnumHasAnyFlags(InFlags, ETextureCreateFlags::DepthStencilTarget | ETextureCreateFlags::DepthStencilResolveTarget))
	{
		OutFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		/*if (EnumHasAnyFlags(InFlags, ETextureCreateFlags::DepthStencilResolveTarget))
			OutFlags |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;*/
	}

	return OutFlags;
}


void FVulkanTexture::DoTransition(VkCommandBuffer InCmdBuffer, VkImageLayout TargetLayout)
{
	if(ImageLayout == TargetLayout)
		return;

	REV_CORE_ASSERT(InCmdBuffer);
	FVulkanUtils::TransitionImage(InCmdBuffer, Image, ImageLayout, TargetLayout, ImageAspectFlags);
	ImageLayout = TargetLayout;
}

void FVulkanTexture::ClearContent(FVulkanContext* Context, uint8 InMipLevel, uint8 InMipCount, uint16 InArrayIndex, uint16 InArrayCount)
{
	REV_CORE_ASSERT(InMipLevel < TextureDesc.NumMips, "MipLevel out of range");
	FVulkanUtils::ImmediateClearImage(Context, Image, ImageAspectFlags, GetClearValue(), InMipLevel, InMipCount, InArrayIndex, InArrayCount);
}

void FVulkanTexture::Resize(uint32 InWidth, uint32 InHeight, uint32 InDepth)
{
	if (InWidth != TextureDesc.Width || InHeight != TextureDesc.Height || InDepth != TextureDesc.Depth)
	{
		Release();
		TextureDesc.Width = InWidth;
		TextureDesc.Height = InHeight;
		TextureDesc.Depth = InDepth;
		Init();
	}
}

//DynamicRHI
Ref<FRHITexture> FVulkanDynamicRHI::RHICreateTexture(const FRHITextureDesc& InDesc)
{
	switch (InDesc.Dimension)
	{
	case ETextureDimension::Texture2D:
		return CreateRef<FVulkanTexture2D>(InDesc);
	case ETextureDimension::Texture2DArray:
		return CreateRef<FVulkanTexture2DArray>(InDesc);
	case ETextureDimension::TextureCube:
		return CreateRef<FVulkanTextureCube>(InDesc);
	case ETextureDimension::TextureCubeArray:
		//return CreateRef<FOpenGLTextureCubeArray>(InDesc, InSamplerDesc);
	case ETextureDimension::Texture3D:
		//return CreateRef<FOpenGLTexture3D>(InDesc, InSamplerDesc);
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


