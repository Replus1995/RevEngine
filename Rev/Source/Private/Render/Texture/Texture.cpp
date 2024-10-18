#include "Rev/Render/Texture/Texture.h"
#include "Rev/Render/RHI/RHITexture.h"

namespace Rev
{

Texture::Texture(const Ref<FRHITexture>& InResource)
	: mTextureRHI(InResource)
{
}

Texture::~Texture()
{
}

uint16 Texture::Width() const
{
	return mTextureRHI ? mTextureRHI->GetWidth() : 0;
}

uint16 Texture::Height() const
{
	return mTextureRHI ? mTextureRHI->GetHeight() : 0;
}

uint16 Texture::Depth() const
{
	return mTextureRHI ? mTextureRHI->GetDepth() : 0;
}

uint16 Texture::ArraySize() const
{
	return mTextureRHI ? mTextureRHI->GetArraySize() : 0;
}

EPixelFormat Texture::Format() const
{
	return mTextureRHI ? mTextureRHI->GetFormat() : EPixelFormat::PF_Unknown;
}

bool Texture::IsTexture2D() const
{
	return mTextureRHI ? mTextureRHI->GetDesc().Dimension == ETextureDimension::Texture2D : false;
}

bool Texture::IsTexture2DArray() const
{
	return mTextureRHI ? mTextureRHI->GetDesc().Dimension == ETextureDimension::Texture2DArray : false;
}

bool Texture::IsTextureCube() const
{
	return mTextureRHI ? mTextureRHI->GetDesc().Dimension == ETextureDimension::TextureCube : false;
}

bool Texture::IsTextureCubeArray() const
{
	return mTextureRHI ? mTextureRHI->GetDesc().Dimension == ETextureDimension::TextureCubeArray : false;
}

bool Texture::IsTexture3D() const
{
	return mTextureRHI ? mTextureRHI->GetDesc().Dimension == ETextureDimension::Texture3D : false;
}

}