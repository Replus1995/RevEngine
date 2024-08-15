#include "OpenGLTexture2D.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FOpenGLTexture2D::FOpenGLTexture2D(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc)
	: FOpenGLTexture(InDesc, InSamplerDesc)
{
	REV_CORE_ASSERT(InDesc.Dimension == ETextureDimension::Texture2D);
	CreateResource();
}

FOpenGLTexture2D::~FOpenGLTexture2D()
{
	glDeleteTextures(1, &mHandle);
}

void FOpenGLTexture2D::UpdateLayerData(const void* InData, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex, int32 InDepth)
{
	if (mDesc.NumSamples != 1)
	{
		REV_CORE_ERROR("Updating multisample texture is not allowed");
		return;
	}
	REV_CORE_ASSERT(InDepth <= 0, "Depth must be 0(-1) for 2d texture");
	REV_CORE_ASSERT(InArrayIndex == 0, "ArrayIndex must be 0 for 2d texture");
	REV_CORE_ASSERT(InMipLevel < mDesc.NumMips, "MipLevel out of range");

	auto [MipWidth, MipHeight] = CalculateMipSize2D(InMipLevel);
	REV_CORE_ASSERT(InSize == MipWidth * MipWidth * mFormatData.PixelSize, "Data size mismatch");

	glTextureSubImage2D(mHandle, InMipLevel, 0, 0, MipWidth, MipHeight, mFormatData.DataFormat, mFormatData.DataType, InData);
}

void FOpenGLTexture2D::ClearLayerData(uint8 InMipLevel, uint16 InArrayIndex, int32 InDepth)
{
	REV_CORE_ASSERT(InDepth <= 0, "Depth must be 0(-1) for 2d texture");
	REV_CORE_ASSERT(InArrayIndex == 0, "ArrayIndex must be 0 for 2d texture");
	REV_CORE_ASSERT(InMipLevel < mDesc.NumMips, "MipLevel out of range");

	FClearColorBuffer ColorBuffer;
	FillClearColor(ColorBuffer);
	auto [MipWidth, MipHeight] = CalculateMipSize2D(InMipLevel);
	glClearTexSubImage(mHandle, InMipLevel, 0, 0, 0, MipWidth, MipHeight, 1, mFormatData.DataFormat, mFormatData.DataType, (const void*)ColorBuffer.Data());
}

void FOpenGLTexture2D::CreateResource()
{
	if (mDesc.NumSamples == 1)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &mHandle);
		glTextureStorage2D(mHandle, mDesc.NumMips, mFormatData.InternalFormat, GetWidth(), GetHeight());
	}
	else
	{
		REV_CORE_ASSERT(mDesc.NumSamples > 1 && mDesc.NumMips == 1);
		glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &mHandle);
		glTexStorage2DMultisample(mHandle, mDesc.NumSamples, mFormatData.InternalFormat, GetWidth(), GetHeight(), GL_TRUE);
	}
}

}