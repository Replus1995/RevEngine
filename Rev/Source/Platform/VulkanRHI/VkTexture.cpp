﻿#include "VkTexture.h"
#include "VkCore.h"
#include "VkUtils.h"
#include "Rev/Core/Assert.h"

#include "VkSampler.h"
#include "VkTexture2D.h"

namespace Rev
{

const FRHISampler* FVulkanTexture::GetSampler() const
{
	return mSampler.get();
}

void FVulkanTexture::ClearMipData(uint8 InMipLevel)
{
	REV_CORE_ASSERT(InMipLevel < mDesc.NumMips, "MipLevel out of range");
	FVulkanUtils::ImmediateClearImage(mImage, mFormatInfo.AspectFlags, GetClearValue(), InMipLevel, 1, 0, 0);
}

void FVulkanTexture::Transition(VkImageLayout DstLayout, VkCommandBuffer InCmdBuffer)
{
	VkCommandBuffer CmdBuffer = InCmdBuffer ? InCmdBuffer : FVulkanCore::GetMainCmdBuffer();
	REV_CORE_ASSERT(CmdBuffer);
	FVulkanUtils::TransitionImage(CmdBuffer, mImage, mImageLayout, DstLayout);
	mImageLayout = DstLayout;
}

void FVulkanTexture::Transition(VkImageLayout SrcLayout, VkImageLayout DstLayout, VkCommandBuffer InCmdBuffer)
{
	VkCommandBuffer CmdBuffer = InCmdBuffer ? InCmdBuffer : FVulkanCore::GetMainCmdBuffer();
    REV_CORE_ASSERT(CmdBuffer);
    FVulkanUtils::TransitionImage(CmdBuffer, mImage, SrcLayout, DstLayout);
    mImageLayout = DstLayout;
}

FVulkanTexture::FVulkanTexture(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc)
    : FRHITexture(InDesc)
    , mFormatInfo(FVkPixelFormat::TranslatePixelFormat(InDesc.Format, InDesc.bSRGB))
{
	REV_CORE_ASSERT(mFormatInfo.AspectFlags != VK_IMAGE_ASPECT_NONE);
	mSampler = CreateVkSampler(InSamplerDesc);
}

VkExtent3D FVulkanTexture::GetExtent()
{
    return { mDesc.Width, mDesc.Height, mDesc.Depth };
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
	bool bColorImage = mFormatInfo.AspectFlags & VK_IMAGE_ASPECT_COLOR_BIT;
	VkClearValue ClearValue;
	if (bColorImage)
	{
		VkClearColorValue ClearColor{};
		for (size_t i = 0; i < 4; i++)
		{
			ClearValue.color.float32[i] = mDesc.ClearColor.RGBA[i];
			switch (mFormatInfo.PixelDepth)
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

Ref<FVulkanTexture> CreateVkTexture(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc)
{
	switch (InDesc.Dimension)
	{
	case ETextureDimension::Texture2D:
		return CreateRef<FVulkanTexture2D>(InDesc, InSamplerDesc);
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

