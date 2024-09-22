#include "Rev/Render/Texture/Texture.h"
#include "Rev/Render/RHI/RHISampler.h"
#include "Rev/Render/RHI/RHITexture.h"

namespace Rev
{

Texture::Texture(const Ref<FRHITexture>& InResource)
	: mResource(InResource)
{
}

Texture::~Texture()
{
}

uint16 Texture::Width() const
{
	return mResource ? mResource->GetWidth() : 0;
}

uint16 Texture::Height() const
{
	return mResource ? mResource->GetHeight() : 0;
}

uint16 Texture::Depth() const
{
	return mResource ? mResource->GetDepth() : 0;
}

uint16 Texture::ArraySize() const
{
	return mResource ? mResource->GetArraySize() : 0;
}

EPixelFormat Texture::Format() const
{
	return mResource ? mResource->GetFormat() : EPixelFormat::PF_Unknown;
}

bool Texture::IsTexture2D() const
{
	return mResource ? mResource->GetDesc().Dimension == ETextureDimension::Texture2D : false;
}

bool Texture::IsTexture2DArray() const
{
	return mResource ? mResource->GetDesc().Dimension == ETextureDimension::Texture2DArray : false;
}

bool Texture::IsTextureCube() const
{
	return mResource ? mResource->GetDesc().Dimension == ETextureDimension::TextureCube : false;
}

bool Texture::IsTextureCubeArray() const
{
	return mResource ? mResource->GetDesc().Dimension == ETextureDimension::TextureCubeArray : false;
}

bool Texture::IsTexture3D() const
{
	return mResource ? mResource->GetDesc().Dimension == ETextureDimension::Texture3D : false;
}

}