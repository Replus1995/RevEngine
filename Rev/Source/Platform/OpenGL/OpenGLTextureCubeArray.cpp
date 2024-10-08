#include "OpenGLTextureCubeArray.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FOpenGLTextureCubeArray::FOpenGLTextureCubeArray(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc)
	: FOpenGLTexture(InDesc, InSamplerDesc)
{
	RE_CORE_ASSERT(InDesc.Dimension == ETextureDimension::TextureCubeArray);
	CreateResource();
}

FOpenGLTextureCubeArray::~FOpenGLTextureCubeArray()
{
	glDeleteTextures(1, &mHandle);
}

void FOpenGLTextureCubeArray::UpdateLayerData(const void* InData, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex, int32 InDepth)
{
	//Update one mip of one face of one cube texture at a time
	RE_CORE_ASSERT(InArrayIndex < mDesc.ArraySize, "ArrayIndex out of range");
	RE_CORE_ASSERT(InMipLevel < mDesc.NumMips, "MipLevel out of range");

	auto [MipWidth, MipHeight] = CalculateMipSize2D(InMipLevel);
	if (InDepth < 0)
	{
		RE_CORE_ASSERT(InSize == MipWidth * MipWidth * 6 * mFormatData.PixelSize, "Data size mismatch");
		uint32 CubeFaceIndex = InArrayIndex * 6;
		glTextureSubImage3D(mHandle, InMipLevel, 0, 0, CubeFaceIndex, MipWidth, MipHeight, 6, mFormatData.DataFormat, mFormatData.DataType, InData);
	}
	else
	{
		RE_CORE_ASSERT(InDepth < TCF_Count, "Depth for cube must be in range 0~5");
		RE_CORE_ASSERT(InSize == MipWidth * MipWidth * mFormatData.PixelSize, "Data size mismatch");
		uint32 CubeFaceIndex = InArrayIndex * 6 + InDepth;
		glTextureSubImage3D(mHandle, InMipLevel, 0, 0, CubeFaceIndex, MipWidth, MipHeight, 1, mFormatData.DataFormat, mFormatData.DataType, InData);
	}
}

void FOpenGLTextureCubeArray::ClearLayerData(uint8 InMipLevel, uint16 InArrayIndex, int32 InDepth)
{
	RE_CORE_ASSERT(InArrayIndex < mDesc.ArraySize, "ArrayIndex out of range");
	RE_CORE_ASSERT(InMipLevel < mDesc.NumMips, "MipLevel out of range");

	FClearColorBuffer ColorBuffer;
	FillClearColor(ColorBuffer);
	auto [MipWidth, MipHeight] = CalculateMipSize2D(InMipLevel);
	if (InDepth < 0)
	{
		uint32 CubeFaceIndex = InArrayIndex * 6;
		glClearTexSubImage(mHandle, InMipLevel, 0, 0, CubeFaceIndex, MipWidth, MipHeight, 6, mFormatData.DataFormat, mFormatData.DataType, (const void*)ColorBuffer.Data());
	}
	else
	{
		RE_CORE_ASSERT(InDepth < TCF_Count, "Depth for cube must be in range 0~5");
		uint32 CubeFaceIndex = InArrayIndex * 6 + InDepth;
		glClearTexSubImage(mHandle, InMipLevel, 0, 0, CubeFaceIndex, MipWidth, MipHeight, 1, mFormatData.DataFormat, mFormatData.DataType, (const void*)ColorBuffer.Data());
	}
}

void FOpenGLTextureCubeArray::CreateResource()
{
	glCreateTextures(GL_TEXTURE_CUBE_MAP_ARRAY, 1, &mHandle);
	glTextureStorage3D(mHandle, mDesc.NumMips, mFormatData.InternalFormat,  GetWidth(), GetHeight(), mDesc.ArraySize * 6);
}

}