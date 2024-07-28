#include "OpenGLTexture2D.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FOpenGLTexture2D::FOpenGLTexture2D(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc)
	: FOpenGLTexture(InDesc, InSamplerDesc)
{
	CreateResource();
}

FOpenGLTexture2D::~FOpenGLTexture2D()
{
	glDeleteTextures(1, &mHandle);
}

void FOpenGLTexture2D::UpdateData(const void* InData, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex)
{
	RE_CORE_ASSERT(InArrayIndex == 0, "ArrayIndex must be 0 for 2d textures");
	RE_CORE_ASSERT(InMipLevel < mDesc.NumMips, "MipLevel out of range");

	uint32 MipFactor = Math::PowI<uint32>(2, InMipLevel);
	uint32 MipWidth = GetWidth() / MipFactor;
	uint32 MipHeight = GetHeight() / MipFactor;
	RE_CORE_ASSERT(InSize == MipWidth * MipWidth * mFormatData.PixelSize, "Data size mismatch");

	glTextureSubImage2D(mHandle, InMipLevel, 0, 0, MipWidth, MipHeight, mFormatData.DataFormat, mFormatData.DataType, InData);
}

void FOpenGLTexture2D::CreateResource()
{
	glCreateTextures(GL_TEXTURE_2D, 1, &mHandle);
	glTextureStorage2D(mHandle, mDesc.NumMips, mFormatData.InternalFormat,  GetWidth(), GetHeight());
}

}