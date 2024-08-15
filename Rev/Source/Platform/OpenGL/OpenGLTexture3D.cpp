#include "OpenGLTexture3D.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FOpenGLTexture3D::FOpenGLTexture3D(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc)
	: FOpenGLTexture(InDesc, InSamplerDesc)
{
	REV_CORE_ASSERT(InDesc.Dimension == ETextureDimension::Texture3D);
	CreateResource();
}

FOpenGLTexture3D::~FOpenGLTexture3D()
{
	glDeleteTextures(1, &mHandle);
}

void FOpenGLTexture3D::UpdateLayerData(const void* InData, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex, int32 InDepth)
{
	//Update one mip at a time
	REV_CORE_ASSERT(InArrayIndex == 0, "ArrayIndex must be 0 for 3d texture");
	REV_CORE_ASSERT(InMipLevel < mDesc.NumMips, "MipLevel out of range");

	auto [MipWidth, MipHeight, MipDepth] = CalculateMipSize3D(InMipLevel);
	if (InDepth < 0)
	{
		REV_CORE_ASSERT(InSize == MipWidth * MipWidth * MipDepth * mFormatData.PixelSize, "Data size mismatch");
		glTextureSubImage3D(mHandle, InMipLevel, 0, 0, 0, MipWidth, MipHeight, MipDepth, mFormatData.DataFormat, mFormatData.DataType, InData);
	}
	else
	{
		REV_CORE_ASSERT(InDepth < MipDepth, "Depth out of range");
		REV_CORE_ASSERT(InSize == MipWidth * MipWidth * mFormatData.PixelSize, "Data size mismatch");
		glTextureSubImage3D(mHandle, InMipLevel, 0, 0, InDepth, MipWidth, MipHeight, 1, mFormatData.DataFormat, mFormatData.DataType, InData);
	}
}

void FOpenGLTexture3D::ClearLayerData(uint8 InMipLevel, uint16 InArrayIndex, int32 InDepth)
{
	REV_CORE_ASSERT(InArrayIndex == 0, "ArrayIndex must be 0 for 3d texture");
	REV_CORE_ASSERT(InMipLevel < mDesc.NumMips, "MipLevel out of range");

	FClearColorBuffer ColorBuffer;
	FillClearColor(ColorBuffer);
	auto [MipWidth, MipHeight, MipDepth] = CalculateMipSize3D(InMipLevel);
	if (InDepth < 0)
	{
		glClearTexSubImage(mHandle, InMipLevel, 0, 0, 0, MipWidth, MipHeight, MipDepth, mFormatData.DataFormat, mFormatData.DataType, (const void*)ColorBuffer.Data());
	}
	else
	{
		REV_CORE_ASSERT(InDepth < MipDepth, "Depth out of range");
		glClearTexSubImage(mHandle, InMipLevel, 0, 0, InDepth, MipWidth, MipHeight, 1, mFormatData.DataFormat, mFormatData.DataType, (const void*)ColorBuffer.Data());
	}
}

void FOpenGLTexture3D::CreateResource()
{
	glCreateTextures(GL_TEXTURE_3D, 1, &mHandle);
	glTextureStorage3D(mHandle, mDesc.NumMips, mFormatData.InternalFormat,  GetWidth(), GetHeight(), mDesc.Depth);
}

}