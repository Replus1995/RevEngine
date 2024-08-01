#include "Rev/Asset/TextureStorage.h"
#include "Rev/Render/RHI/RHIResourceFactory.h"

namespace Rev
{

uint8 FImageStorage::NumMips()
{
	return mImages.empty() ? 0 : (uint8)mImages[0].size();
}

uint8 FImageStorage::NumLayers()
{
	return (uint8)mImages.size();
}

void FImageStorage::Resize(uint8 NumMips, uint8 NumLayers)
{
	mImages.resize(NumLayers);
	for (uint8 i = 0; i < NumLayers; i++)
	{
		mImages[i].resize(NumMips);
	}
}

FBuffer& FImageStorage::At(uint8 MipIndex, uint8 LayerIndex)
{
	RE_CORE_ASSERT(MipIndex < NumMips() && LayerIndex < NumLayers());
	return mImages[LayerIndex][MipIndex];
}

Ref<Texture> FTextureStorage::CreateTexture(bool bForceSRGB)
{
	if(mCache)
		return mCache;

	TextureDesc.bSRGB = bForceSRGB ? true : TextureDesc.bSRGB;
	if (TextureDesc.Dimension == ETextureDimension::TextureCube && ImageData.NumLayers() == 6)
	{
		RE_CORE_ASSERT(TextureDesc.NumMips == ImageData.NumMips());
		mCache = CreateRef<Texture>(FRHIResourceFactory::CreateTexture(TextureDesc, SamplerDesc));
		for (uint8 iCubeFace = 0; iCubeFace < 6; iCubeFace++)
		{
			for (uint8 iMip = 0; iMip < TextureDesc.NumMips; iMip++)
			{
				FBuffer& ImageBuffer = ImageData.At(iMip, iCubeFace);
				mCache->GetResource()->UpdateLayerData(ImageBuffer.Data(), ImageBuffer.Size(), iMip, 0, iCubeFace);
			}
		}
		return mCache;
	}
	else if (TextureDesc.Dimension == ETextureDimension::TextureCubeArray && ImageData.NumLayers() == 6 * TextureDesc.ArraySize)
	{
		RE_CORE_ASSERT(TextureDesc.NumMips == ImageData.NumMips());
		mCache = CreateRef<Texture>(FRHIResourceFactory::CreateTexture(TextureDesc, SamplerDesc));
		for (uint8 iLayer = 0; iLayer < TextureDesc.ArraySize; iLayer++)
		{
			for (uint8 iCubeFace = 0; iCubeFace < 6; iCubeFace++)
			{
				for (uint8 iMip = 0; iMip < TextureDesc.NumMips; iMip++)
				{
					FBuffer& ImageBuffer = ImageData.At(iMip, iLayer * 6 + iCubeFace);
					mCache->GetResource()->UpdateLayerData(ImageBuffer.Data(), ImageBuffer.Size(), iMip, iLayer, iCubeFace);
				}
			}
		}
		return mCache;
	}
	
	RE_CORE_ASSERT(TextureDesc.NumMips == ImageData.NumMips());
	RE_CORE_ASSERT(TextureDesc.ArraySize == ImageData.NumLayers());

	
	mCache = CreateRef<Texture>(FRHIResourceFactory::CreateTexture(TextureDesc, SamplerDesc));
	for (uint8 iLayer = 0; iLayer < TextureDesc.ArraySize; iLayer++)
	{
		for (uint8 iMip = 0; iMip < TextureDesc.NumMips; iMip++)
		{
			FBuffer& ImageBuffer = ImageData.At(iMip, iLayer);
			mCache->GetResource()->UpdateLayerData(ImageBuffer.Data(), ImageBuffer.Size(), iMip, iLayer);
		}
	}
	return mCache;
}


}


