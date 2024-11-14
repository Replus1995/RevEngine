#pragma once
#include "Rev/Render/Resource/TextureResource.h"
#include "Rev/Render/RHI/RHITexture.h"

namespace Rev
{

uint32 FTexture::GetSizeX() const
{
	return TextureRHI ? TextureRHI->GetWidth() : 0;
}

uint32 FTexture::GetSizeY() const
{
	return TextureRHI ? TextureRHI->GetHeight() : 0;
}

uint32 FTexture::GetSizeZ() const
{
	return TextureRHI ? TextureRHI->GetDepth() : 0;
}

}