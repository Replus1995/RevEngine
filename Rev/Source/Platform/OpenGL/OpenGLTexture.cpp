#include "OpenGLTexture.h"
#include "Rev/Core/Assert.h"

#include "OpenGLSampler.h"
#include "OpenGLTexture2D.h"
#include "OpenGLTexture2DArray.h"
#include "OpenGLTextureCube.h"
#include "OpenGLTextureCubeArray.h"
#include "OpenGLTexture3D.h"

namespace Rev
{

const FRHISampler* FOpenGLTexture::GetSampler() const
{
	return mSampler.get();
}

void FOpenGLTexture::ClearMipData(uint8 InMipLevel)
{
	REV_CORE_ASSERT(InMipLevel < mDesc.NumMips, "MipLevel out of range");
	FClearColorBuffer ColorBuffer;
	FillClearColor(ColorBuffer);
	glClearTexImage(mHandle, InMipLevel, mFormatData.DataFormat, mFormatData.DataType, (const void*)ColorBuffer.Data());
}

FOpenGLTexture::FOpenGLTexture(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc)
	: FRHITexture(InDesc)
	, mFormatData(FOpenGLPixelFormat::TranslatePixelFormat(InDesc.Format, InDesc.bSRGB))
{
	mSampler = CreateRef<FOpenGLSampler>(InSamplerDesc, mFormatData.DataType, InDesc.NumMips > 1);
}

std::pair<uint32, uint32> FOpenGLTexture::CalculateMipSize2D(uint32 InMipLevel)
{
	return { std::max<uint32>(1, GetWidth() >> InMipLevel), std::max<uint32>(1, GetHeight() >> InMipLevel) };
}

std::tuple<uint32, uint32, uint32> FOpenGLTexture::CalculateMipSize3D(uint32 InMipLevel)
{
	return { std::max<uint32>(1, GetWidth() >> InMipLevel), std::max<uint32>(1, GetHeight() >> InMipLevel), std::max<uint32>(1, GetDepth() >> InMipLevel)};
}

void FOpenGLTexture::FillClearColor(FClearColorBuffer& OutBuffer)
{
	switch (mDesc.Format)
	{
	case PF_R32G32B32A32F:
	{
		Math::FLinearColor& ClearColor = *OutBuffer.DataAs<Math::FLinearColor>();
		ClearColor = mDesc.ClearColor.RGBA;
		break;
	}
	case PF_R8G8B8A8:
	case PF_RGB8:
	{
		Math::FColor& ClearColor = *OutBuffer.DataAs<Math::FColor>();
		ClearColor = Math::FLinearColor::ToColor(mDesc.ClearColor.RGBA, mDesc.bSRGB);
		break;
	}
	case PF_R8:
	{
		uint8& ClearColor = *OutBuffer.DataAs<uint8>();
		ClearColor = mDesc.ClearColor.RGBA.R * 255.0f;
		break;
	}
	case PF_R16:
	{
		uint16& ClearColor = *OutBuffer.DataAs<uint16>();
		ClearColor = mDesc.ClearColor.RGBA.R * 65535.0f;
		break;
	}
	case PF_DepthStencil:
	{
		uint32& ClearColor = *OutBuffer.DataAs<uint32>();
		uint32 Depth = uint32(mDesc.ClearColor.Depth * 16777215.0f) << 8;
		uint8 Stencil = mDesc.ClearColor.Stencil;
		ClearColor = Depth & (uint32)Stencil;
		break;
	}
	case PF_ShadowDepth:
	{
		float& ClearColor = *OutBuffer.DataAs<float>();
		ClearColor = mDesc.ClearColor.Depth;
		break;
	}
	default:
		break;
	}
}

Ref<FOpenGLTexture> CreateOpenGLTexture(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc)
{
	switch (InDesc.Dimension)
	{
	case ETextureDimension::Texture2D:
		return CreateRef<FOpenGLTexture2D>(InDesc, InSamplerDesc);
	case ETextureDimension::Texture2DArray:
		return CreateRef<FOpenGLTexture2DArray>(InDesc, InSamplerDesc);
	case ETextureDimension::TextureCube:
		return CreateRef<FOpenGLTextureCube>(InDesc, InSamplerDesc);
	case ETextureDimension::TextureCubeArray:
		return CreateRef<FOpenGLTextureCubeArray>(InDesc, InSamplerDesc);
	case ETextureDimension::Texture3D:
		return CreateRef<FOpenGLTexture3D>(InDesc, InSamplerDesc);
	default:
		break;
	}
	REV_CORE_ASSERT(false, "Unsupported Texture Dimension!");
	return nullptr;
}

}


