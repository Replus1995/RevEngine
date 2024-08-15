#include "OpenGLTextureCube.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FOpenGLTextureCube::FOpenGLTextureCube(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc)
	: FOpenGLTexture(InDesc, InSamplerDesc)
{
	REV_CORE_ASSERT(InDesc.Dimension == ETextureDimension::TextureCube);
	CreateResource();
}

FOpenGLTextureCube::~FOpenGLTextureCube()
{
	glDeleteTextures(1, &mHandle);
}

void FOpenGLTextureCube::UpdateLayerData(const void* InData, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex, int32 InDepth)
{
	//Update one mip of one face of one cube texture at a time
	REV_CORE_ASSERT(InArrayIndex == 0, "ArrayIndex must be 0 for cube texture");
	REV_CORE_ASSERT(InMipLevel < mDesc.NumMips, "MipLevel out of range");

	auto [MipWidth, MipHeight] = CalculateMipSize2D(InMipLevel);
	if (InDepth < 0)
	{
		REV_CORE_ASSERT(InSize == MipWidth * MipWidth * 6 * mFormatData.PixelSize, "Data size mismatch");
		glTextureSubImage3D(mHandle, InMipLevel, 0, 0, 0, MipWidth, MipHeight, 6, mFormatData.DataFormat, mFormatData.DataType, InData);
	}
	else
	{
		REV_CORE_ASSERT(InDepth < TCF_Count, "Depth for cube must be in range 0~5");
		REV_CORE_ASSERT(InSize == MipWidth * MipWidth * mFormatData.PixelSize, "Data size mismatch");
		uint32 CubeFaceIndex = InDepth;
		glTextureSubImage3D(mHandle, InMipLevel, 0, 0, CubeFaceIndex, MipWidth, MipHeight, 1, mFormatData.DataFormat, mFormatData.DataType, InData);
	}
}

void FOpenGLTextureCube::ClearLayerData(uint8 InMipLevel, uint16 InArrayIndex, int32 InDepth)
{
	REV_CORE_ASSERT(InArrayIndex == 0, "ArrayIndex must be 0 for cube texture");
	REV_CORE_ASSERT(InMipLevel < mDesc.NumMips, "MipLevel out of range");

	FClearColorBuffer ColorBuffer;
	FillClearColor(ColorBuffer);
	auto [MipWidth, MipHeight] = CalculateMipSize2D(InMipLevel);
	if (InDepth < 0)
	{
		glClearTexSubImage(mHandle, InMipLevel, 0, 0, 0, MipWidth, MipHeight, 6, mFormatData.DataFormat, mFormatData.DataType, (const void*)ColorBuffer.Data());
	}
	else
	{
		REV_CORE_ASSERT(InDepth < TCF_Count, "Depth for cube must be in range 0~5");
		uint32 CubeFaceIndex = InDepth;
		glClearTexSubImage(mHandle, InMipLevel, 0, 0, CubeFaceIndex, MipWidth, MipHeight, 1, mFormatData.DataFormat, mFormatData.DataType, (const void*)ColorBuffer.Data());
	}
}

void FOpenGLTextureCube::CreateResource()
{
	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &mHandle);
	glTextureStorage2D(mHandle, mDesc.NumMips, mFormatData.InternalFormat,  GetWidth(), GetHeight());
}

}