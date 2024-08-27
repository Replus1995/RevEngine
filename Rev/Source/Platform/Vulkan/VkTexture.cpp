#include "VkTexture.h"

namespace Rev
{

FVkTexture::FVkTexture(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc)
    : FRHITexture(InDesc)
    , mFormatInfo(FVkPixelFormat::TranslatePixelFormat(InDesc.Format, InDesc.bSRGB))
{
}

VkExtent3D FVkTexture::GetExtent()
{
    return { mDesc.Width, mDesc.Height, mDesc.Depth };
}

VkExtent2D FVkTexture::CalculateMipSize2D(uint32 InMipLevel)
{
    return { std::max<uint32>(1, GetWidth() >> InMipLevel), std::max<uint32>(1, GetHeight() >> InMipLevel) };
}

VkExtent3D FVkTexture::CalculateMipSize3D(uint32 InMipLevel)
{
    return { std::max<uint32>(1, GetWidth() >> InMipLevel), std::max<uint32>(1, GetHeight() >> InMipLevel), std::max<uint32>(1, GetDepth() >> InMipLevel) };
}

}


