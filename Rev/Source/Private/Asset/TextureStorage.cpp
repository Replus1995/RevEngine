#include "Rev/Asset/TextureStorage.h"
#include "Rev/Core/Assert.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/Resource/RenderResource.h"

namespace Rev
{

FImageStorage::FImageStorage(FImageStorage&& Other) noexcept
	: mImages(std::move(Other).mImages)
	, mNumMips(Other.mNumMips)
	, mNumLayers(Other.mNumLayers)
{
}


uint8 FImageStorage::NumMips()
{
	return mNumMips;
}

uint16 FImageStorage::NumLayers()
{
	return mNumLayers;
}

void FImageStorage::Resize(uint8 NumMips, uint16 NumLayers)
{
	mImages.resize(NumLayers * NumLayers);
	mNumMips = NumMips;
	mNumLayers = NumLayers;
}

FBuffer& FImageStorage::At(uint8 MipIndex, uint16 LayerIndex)
{
	REV_CORE_ASSERT(MipIndex < mNumMips && LayerIndex < mNumLayers);
	uint32 Index = LayerIndex * mNumMips + MipIndex;
	return mImages[Index];
}

FTextureStorage::FTextureStorage(FTextureStorage&& Other) noexcept
	: Name(Other.Name)
	, SamplerDesc(std::move(Other.SamplerDesc))
	, TextureDesc(std::move(Other.TextureDesc))
	, ImageData(std::move(Other.ImageData))
{
}

Ref<FTexture> FTextureStorage::CreateTexture(bool bForceSRGB)
{
	if(mCache)
		return mCache;

	if(bForceSRGB)
		TextureDesc.AddFlags(ETextureCreateFlags::SRGB);

	if (TextureDesc.Dimension == ETextureDimension::TextureCube && ImageData.NumLayers() == 6)
	{
		REV_CORE_ASSERT(TextureDesc.NumMips == ImageData.NumMips());
		mCache = CreateRef<FTexture>();
		mCache->TextureRHI = GDynamicRHI->CreateTexture(TextureDesc);
		for (uint8 iCubeFace = 0; iCubeFace < 6; iCubeFace++)
		{
			for (uint8 iMip = 0; iMip < TextureDesc.NumMips; iMip++)
			{
				FBuffer& ImageBuffer = ImageData.At(iMip, iCubeFace);
				FRenderCore::GetMainContext()->UpdateTexture(mCache->GetTextureRHI(), ImageBuffer.Data(), ImageBuffer.Size(), iMip, iCubeFace);
			}
		}
		return mCache;
	}
	else if (TextureDesc.Dimension == ETextureDimension::TextureCubeArray && ImageData.NumLayers() == 6 * TextureDesc.ArraySize)
	{
		REV_CORE_ASSERT(TextureDesc.NumMips == ImageData.NumMips());
		mCache = CreateRef<FTexture>();
		mCache->TextureRHI = GDynamicRHI->CreateTexture(TextureDesc);
		for (uint8 iLayer = 0; iLayer < TextureDesc.ArraySize; iLayer++)
		{
			for (uint8 iCubeFace = 0; iCubeFace < 6; iCubeFace++)
			{
				for (uint8 iMip = 0; iMip < TextureDesc.NumMips; iMip++)
				{
					FBuffer& ImageBuffer = ImageData.At(iMip, iLayer * 6 + iCubeFace);
					FRenderCore::GetMainContext()->UpdateTexture(mCache->GetTextureRHI(), ImageBuffer.Data(), ImageBuffer.Size(), iLayer * 6 + iCubeFace);
				}
			}
		}
		return mCache;
	}
	
	REV_CORE_ASSERT(TextureDesc.NumMips == ImageData.NumMips());
	REV_CORE_ASSERT(TextureDesc.ArraySize == ImageData.NumLayers());

	
	mCache = CreateRef<FTexture>();
	mCache->TextureRHI = GDynamicRHI->CreateTexture(TextureDesc);
	for (uint8 iLayer = 0; iLayer < TextureDesc.ArraySize; iLayer++)
	{
		for (uint8 iMip = 0; iMip < TextureDesc.NumMips; iMip++)
		{
			FBuffer& ImageBuffer = ImageData.At(iMip, iLayer);
			FRenderCore::GetMainContext()->UpdateTexture(mCache->GetTextureRHI(), ImageBuffer.Data(), ImageBuffer.Size(), iMip, iLayer);
		}
	}
	return mCache;
}


}


