#include "OpenGLTexture3D.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FOpenGLTexture3D::FOpenGLTexture3D(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc)
	: FOpenGLTexture(InDesc, InSamplerDesc)
{
	RE_CORE_ASSERT(InDesc.Dimension == ETextureDimension::Texture3D);
	CreateResource();
}

FOpenGLTexture3D::~FOpenGLTexture3D()
{
	glDeleteTextures(1, &mHandle);
}

void FOpenGLTexture3D::UpdateData(const void* InData, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex, uint16 InDepth)
{
	//Update one mip of one 2d texture at a time
	RE_CORE_ASSERT(InDepth < mDesc.Depth, "Depth out of range");
	RE_CORE_ASSERT(InArrayIndex == 0, "ArrayIndex must be 0 for 3d texture");
	RE_CORE_ASSERT(InMipLevel < mDesc.NumMips, "MipLevel out of range");

	uint32 MipWidth = 0, MipHeight = 0;
	CalculateMipSize(InMipLevel, MipWidth, MipHeight);
	RE_CORE_ASSERT(InSize == MipWidth * MipWidth * mFormatData.PixelSize, "Data size mismatch");

	glTextureSubImage3D(mHandle, InMipLevel, 0, 0, InDepth, MipWidth, MipHeight, 1, mFormatData.DataFormat, mFormatData.DataType, InData);
}

void FOpenGLTexture3D::CreateResource()
{
	glCreateTextures(GL_TEXTURE_3D, 1, &mHandle);
	glTextureStorage3D(mHandle, mDesc.NumMips, mFormatData.InternalFormat,  GetWidth(), GetHeight(), mDesc.Depth);
}

}