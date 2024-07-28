#include "OpenGLTextureCube.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FOpenGLTextureCube::FOpenGLTextureCube(const FTextureDesc& InDesc, const FSamplerDesc& InSamplerDesc)
	: FOpenGLTexture(InDesc, InSamplerDesc)
{
	RE_CORE_ASSERT(InDesc.Dimension == ETextureDimension::TextureCube);
	CreateResource();
}

FOpenGLTextureCube::~FOpenGLTextureCube()
{
	glDeleteTextures(1, &mHandle);
}

void FOpenGLTextureCube::UpdateData(const void* InData, uint32 InSize, uint8 InMipLevel, uint16 InArrayIndex, uint16 InDepth)
{
	//Update one mip of one face of one cube texture at a time
	RE_CORE_ASSERT(InDepth < (uint16)ETextureCubeFace::Count, "Depth must be in range 0~5");
	RE_CORE_ASSERT(InArrayIndex == 0, "ArrayIndex must be 0 for cube texture");
	RE_CORE_ASSERT(InMipLevel < mDesc.NumMips, "MipLevel out of range");

	uint32 MipWidth = 0, MipHeight = 0;
	CalculateMipSize(InMipLevel, MipWidth, MipHeight);
	RE_CORE_ASSERT(InSize == MipWidth * MipWidth * mFormatData.PixelSize, "Data size mismatch");

	uint32 CubeFaceIndex = InDepth;
	glTextureSubImage3D(mHandle, InMipLevel, 0, 0, CubeFaceIndex, MipWidth, MipHeight, 1, mFormatData.DataFormat, mFormatData.DataType, InData);
}

void FOpenGLTextureCube::CreateResource()
{
	glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &mHandle);
	glTextureStorage2D(mHandle, mDesc.NumMips, mFormatData.InternalFormat,  GetWidth(), GetHeight());
}

}