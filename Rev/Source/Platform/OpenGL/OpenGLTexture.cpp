#include "OpenGLTexture.h"
#include "Rev/Core/Assert.h"

#include "OpenGLSampler.h"
#include "OpenGLTexture2D.h"

namespace Rev
{
const FRHISampler* FOpenGLTexture::GetSampler() const
{
	return mSampler.get();
}

void FOpenGLTexture::Bind(uint32 InUnit) const
{
	glBindTextureUnit(InUnit, mHandle);
	glBindSampler(InUnit, *(GLuint*)mSampler->GetNativeHandle());
}

void FOpenGLTexture::ClearData()
{
	for (uint8 i = 0; i < mDesc.NumMips; i++)
	{
		ClearMipData(i);
	}
}

FOpenGLTexture::FOpenGLTexture(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc)
	: FRHITexture(InDesc)
	, mFormatData(FOpenGLPixelFormat::TranslatePixelFormat(InDesc.Format, InDesc.bSRGB))
{
	mSampler = CreateRef<FOpenGLSampler>(InSamplerDesc, mFormatData.DataType, InDesc.NumMips > 1);
}

void FOpenGLTexture::ClearMipData(uint32 InMipLevel)
{
	switch (mDesc.Format)
	{
	case PF_R32G32B32A32F:
	{
		const float* ClearColor = mDesc.ClearColor.RGBA.Data();
		glClearTexImage(mHandle, InMipLevel, mFormatData.DataFormat, mFormatData.DataType, (const void*)&ClearColor);
		break;
	}
	case PF_R8G8B8A8:
	case PF_RGB8:
	{
		Math::FColor ClearColor(0, 0, 0, 0);
		if (mDesc.bSRGB)
		{
			ClearColor = Math::FLinearColor::ToSRGB(mDesc.ClearColor.RGBA);
		}
		else
		{
			for (size_t i = 0; i < 4; i++)
			{
				ClearColor[i] = uint8(mDesc.ClearColor.RGBA[i] * 255.0f);
			}
		}
		glClearTexImage(mHandle, InMipLevel, mFormatData.DataFormat, mFormatData.DataType, (const void*)&ClearColor);
		break;
	}
	case PF_R8:
	{
		uint8 ClearColor = mDesc.ClearColor.RGBA.R * 255.0f;
		glClearTexImage(mHandle, InMipLevel, mFormatData.DataFormat, mFormatData.DataType, (const void*)&ClearColor);
		break;
	}
	case PF_R16:
	{
		uint16 ClearColor = mDesc.ClearColor.RGBA.R * 65535.0f;
		glClearTexImage(mHandle, InMipLevel, mFormatData.DataFormat, mFormatData.DataType, (const void*)&ClearColor);
		break;
	}
	case PF_DepthStencil:
	{
		uint32 Depth = uint32(mDesc.ClearColor.Depth * 16777215.0f) << 8;
		uint8 Stencil = mDesc.ClearColor.Stencil;
		uint32 ClearColor = Depth & (uint32)Stencil;
		glClearTexImage(mHandle, InMipLevel, mFormatData.DataFormat, mFormatData.DataType, (const void*)&ClearColor);
		break;
	}
	case PF_ShadowDepth:
	{
		float Depth = mDesc.ClearColor.Depth;
		glClearTexImage(mHandle, InMipLevel, mFormatData.DataFormat, mFormatData.DataType, (const void*)&Depth);
		break;
	}
	default:
		break;
	}
}

void FOpenGLTexture::CalculateMipSize(uint32 InMipLevel, uint32& OutMipWidth, uint32& OutMipHeight)
{
	uint32 MipFactor = Math::PowI<uint32>(2, InMipLevel);
	OutMipWidth = std::max<uint32>(1, GetWidth() / MipFactor);
	OutMipHeight = std::max<uint32>(1, GetHeight() / MipFactor);
}

Ref<FOpenGLTexture> CreateOpenGLTexture(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc)
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

}


