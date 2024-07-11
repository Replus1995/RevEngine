#include "Rev/Render/Texture/Texture.h"
#include "Rev/Render/RHI/RHISampler.h"
#include "Rev/Render/RHI/RHITexture.h"

namespace Rev
{

Texture::Texture()
	: mRHITexture(nullptr)
{
}

Texture::~Texture()
{
}

uint16 Texture::Width() const
{
	return mRHITexture ? mRHITexture->GetWidth() : 0;
}

uint16 Texture::Height() const
{
	return mRHITexture ? mRHITexture->GetHeight() : 0;
}

EPixelFormat Texture::Format() const
{
	return mRHITexture ? mRHITexture->GetFormat() : EPixelFormat::PF_Unknown;
}

}