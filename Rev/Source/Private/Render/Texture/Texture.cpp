#include "Rev/Render/Texture/Texture.h"
#include "Rev/Render/RHI/RHISampler.h"
#include "Rev/Render/RHI/RHITexture.h"

namespace Rev
{

Texture::Texture()
	: mResource(nullptr)
{
}

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

EPixelFormat Texture::Format() const
{
	return mResource ? mResource->GetFormat() : EPixelFormat::PF_Unknown;
}

}