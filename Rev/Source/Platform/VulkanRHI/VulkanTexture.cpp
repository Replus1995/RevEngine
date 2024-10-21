#include "VulkanTexture.h"
#include "VulkanUtils.h"
#include "Rev/Core/Assert.h"

#include "VulkanTexture2D.h"

namespace Rev
{

void FVulkanTexture::Transition(VkImageLayout DstLayout, VkCommandBuffer InCmdBuffer)
{
	REV_CORE_ASSERT(InCmdBuffer);
	FVulkanUtils::TransitionImage(InCmdBuffer, mImage, mImageLayout, DstLayout);
	mImageLayout = DstLayout;
}

void FVulkanTexture::Transition(VkImageLayout SrcLayout, VkImageLayout DstLayout, VkCommandBuffer InCmdBuffer)
{
    REV_CORE_ASSERT(InCmdBuffer);
    FVulkanUtils::TransitionImage(InCmdBuffer, mImage, SrcLayout, DstLayout);
    mImageLayout = DstLayout;
}

FVulkanTexture::FVulkanTexture(const FRHITextureDesc& InDesc)
    : FRHITexture(InDesc)
{
	if (FPixelFormatInfo::HasDepth(InDesc.Format))
		mImageAspectFlags |= VK_IMAGE_ASPECT_DEPTH_BIT;
	if (FPixelFormatInfo::HasStencil(InDesc.Format))
		mImageAspectFlags |= VK_IMAGE_ASPECT_STENCIL_BIT;
	if (mImageAspectFlags == VK_IMAGE_ASPECT_NONE)
		mImageAspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
}

VkExtent3D FVulkanTexture::GetExtent()
{
	uint32 Depth = mDesc.Depth <= 0 ? 1 : mDesc.Depth;
    return { mDesc.Width, mDesc.Height, Depth };
}

VkExtent2D FVulkanTexture::CalculateMipSize2D(uint32 InMipLevel)
{
    return { std::max<uint32>(1, GetWidth() >> InMipLevel), std::max<uint32>(1, GetHeight() >> InMipLevel) };
}

VkExtent3D FVulkanTexture::CalculateMipSize3D(uint32 InMipLevel)
{
    return { std::max<uint32>(1, GetWidth() >> InMipLevel), std::max<uint32>(1, GetHeight() >> InMipLevel), std::max<uint32>(1, GetDepth() >> InMipLevel) };
}

VkClearValue FVulkanTexture::GetClearValue()
{
	bool bColorImage = mImageAspectFlags & VK_IMAGE_ASPECT_COLOR_BIT;
	VkClearValue ClearValue;
	if (bColorImage)
	{
		VkClearColorValue ClearColor{};
		for (size_t i = 0; i < 4; i++)
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
		}
	}
	else
	{
		ClearValue.depthStencil.depth = mDesc.ClearColor.Depth;
		ClearValue.depthStencil.stencil = mDesc.ClearColor.Stencil;
	}

	return ClearValue;
}

void FVulkanTexture::ClearContent(FVulkanContext* Context, uint8 InMipLevel, uint8 InMipCount, uint16 InArrayIndex, uint16 InArrayCount)
{
	REV_CORE_ASSERT(InMipLevel < mDesc.NumMips, "MipLevel out of range");
	FVulkanUtils::ImmediateClearImage(Context, mImage, mImageAspectFlags, GetClearValue(), InMipLevel, InMipCount, InArrayIndex, InArrayCount);
}


Ref<FVulkanTexture> CreateVulkanTexture(const FRHITextureDesc& InDesc)
{
	switch (InDesc.Dimension)
	{
	case ETextureDimension::Texture2D:
		return CreateRef<FVulkanTexture2D>(InDesc);
	case ETextureDimension::Texture2DArray:
		//return CreateRef<FOpenGLTexture2DArray>(InDesc, InSamplerDesc);
	case ETextureDimension::TextureCube:
		//return CreateRef<FOpenGLTextureCube>(InDesc, InSamplerDesc);
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

}


