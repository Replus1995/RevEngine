#include "OpenGLTexture2D.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FOpenGLTexture2D::FOpenGLTexture2D(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc)
	: FOpenGLTexture(InDesc, InSamplerDesc)
{
	RE_CORE_ASSERT(InDesc.Dimension == ETextureDimension::Texture2D);
	CreateResource();
}

FOpenGLTexture2D::~FOpenGLTexture2D()
{
	glDeleteTextures(1, &mHandle);
}

void FOpenGLTexture2D::UpdateData(const void* InData, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex, uint16 InDepth)
{
	if (mDesc.NumSamples != 1)
	{
		RE_CORE_ERROR("Updating multisample texture is not allowed");
		return;
	}
	RE_CORE_ASSERT(InDepth == 0, "Depth must be 0 for 2d texture");
	RE_CORE_ASSERT(InArrayIndex == 0, "ArrayIndex must be 0 for 2d texture");
	RE_CORE_ASSERT(InMipLevel < mDesc.NumMips, "MipLevel out of range");

	uint32 MipWidth = 0, MipHeight = 0;
	CalculateMipSize(InMipLevel, MipWidth, MipHeight);
	RE_CORE_ASSERT(InSize == MipWidth * MipWidth * mFormatData.PixelSize, "Data size mismatch");

	glTextureSubImage2D(mHandle, InMipLevel, 0, 0, MipWidth, MipHeight, mFormatData.DataFormat, mFormatData.DataType, InData);
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
		RE_CORE_ASSERT(mDesc.NumSamples > 1 && mDesc.NumMips == 1);
		glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &mHandle);
		glTexStorage2DMultisample(mHandle, mDesc.NumSamples, mFormatData.InternalFormat, GetWidth(), GetHeight(), GL_TRUE);
	}
}

}