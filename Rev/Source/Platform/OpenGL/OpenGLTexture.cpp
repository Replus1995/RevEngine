#include "OpenGLTexture.h"
#include "Rev/Core/Assert.h"

#include "OpenGLSampler.h"
#include "OpenGLTexture2D.h"

namespace Rev
{

void FOpenGLTexture::SetSamplerState(const FSamplerDesc& InSamplerDesc)
{
	if (mSamplerDesc != InSamplerDesc)
	{
		mSamplerDesc = InSamplerDesc;
		FOpenGLSampler::UpdateSampleState(mSamplerDesc, mHandle, mDesc.NumMips > 1);
	}
}

Ref<FOpenGLTexture> FOpenGLTexture::Create(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc)
{
	switch (InDesc.Dimension)
	{
	case ETextureDimension::Texture2D:
		return CreateRef<FOpenGLTexture2D>(InDesc, InSamplerDesc);
	case ETextureDimension::Texture2DArray:
	case ETextureDimension::Texture3D:
	case ETextureDimension::TextureCube:
	case ETextureDimension::TextureCubeArray:
	default:
		break;
	}
	RE_CORE_ASSERT(false, "Unsupported Texture Dimension!");
	return nullptr;
}

FOpenGLTexture::FOpenGLTexture(const FTextureDesc& InDesc)
	: FRHITexture(InDesc)
	, mFormatData(FOpenGLPixelFormat::TranslatePixelFormat(InDesc.Format, InDesc.bSRGB))
{
}

FOpenGLTexture::FOpenGLTexture(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc)
	: FRHITexture(InDesc)
	, mFormatData(FOpenGLPixelFormat::TranslatePixelFormat(InDesc.Format, InDesc.bSRGB))
	, mSamplerDesc(InSamplerDesc)
{
}

}


