#include "OpenGLTexture2DArray.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FOpenGLTexture2DArray::FOpenGLTexture2DArray(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc)
	: FOpenGLTexture(InDesc, InSamplerDesc)
{
	RE_CORE_ASSERT(InDesc.Dimension == ETextureDimension::Texture2DArray);
	CreateResource();
}

FOpenGLTexture2DArray::~FOpenGLTexture2DArray()
{
	glDeleteTextures(1, &mHandle);
}

void FOpenGLTexture2DArray::UpdateLayerData(const void* InData, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex, int32 InDepth)
{
	if (mDesc.NumSamples != 1)
	{
		RE_CORE_ERROR("Updating multisample texture is not allowed");
		return;
	}
	//Update one mip of one texture at a time
	RE_CORE_ASSERT(InDepth <= 0, "Depth must be 0(-1) for 2d array texture");
	RE_CORE_ASSERT(InArrayIndex < mDesc.ArraySize, "ArrayIndex out of range");
	RE_CORE_ASSERT(InMipLevel < mDesc.NumMips, "MipLevel out of range");

	auto [MipWidth, MipHeight] = CalculateMipSize2D(InMipLevel);
	RE_CORE_ASSERT(InSize == MipWidth * MipWidth * mFormatData.PixelSize, "Data size mismatch");

	glTextureSubImage3D(mHandle, InMipLevel, 0, 0, InArrayIndex, MipWidth, MipHeight, 1, mFormatData.DataFormat, mFormatData.DataType, InData);
}

void FOpenGLTexture2DArray::ClearLayerData(uint8 InMipLevel, uint16 InArrayIndex, int32 InDepth)
{
	RE_CORE_ASSERT(InDepth <= 0, "Depth must be 0(-1) for 2d array texture");
	RE_CORE_ASSERT(InArrayIndex < mDesc.ArraySize, "ArrayIndex out of range");
	RE_CORE_ASSERT(InMipLevel < mDesc.NumMips, "MipLevel out of range");

	FClearColorBuffer ColorBuffer;
	FillClearColor(ColorBuffer);
	auto [MipWidth, MipHeight] = CalculateMipSize2D(InMipLevel);
	glClearTexSubImage(mHandle, InMipLevel, 0, 0, InArrayIndex, MipWidth, MipHeight, 1, mFormatData.DataFormat, mFormatData.DataType, (const void*)ColorBuffer.Data());
}

void FOpenGLTexture2DArray::CreateResource()
{
	if (mDesc.NumSamples == 1)
	{
		glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &mHandle);
		glTextureStorage3D(mHandle, mDesc.NumMips, mFormatData.InternalFormat, GetWidth(), GetHeight(), mDesc.ArraySize);
	}
	else
	{
		RE_CORE_ASSERT(mDesc.NumSamples > 1 && mDesc.NumMips == 1);
		glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, 1, &mHandle);
		glTexStorage3DMultisample(mHandle, mDesc.NumSamples, mFormatData.InternalFormat, GetWidth(), GetHeight(), mDesc.ArraySize, GL_TRUE);
	}
}

}