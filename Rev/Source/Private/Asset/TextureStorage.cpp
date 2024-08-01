#include "Rev/Asset/TextureStorage.h"
#include "Rev/Render/RHI/RHIResourceFactory.h"

namespace Rev
{

uint8 FImageStorage::NumMips()
{
	return mImages.empty();
}

uint8 FImageStorage::NumLayers()
{
	return mImages.
}

Ref<Texture> FTextureStorage::CreateTexture(bool bSRGB)
{
	if(mCache)
		return mCache;
	TextureDesc.bSRGB = bSRGB;
	mCache = CreateRef<Texture>(FRHIResourceFactory::CreateTexture(TextureDesc, SamplerDesc));


	mCache->GetResource()->UpdateLayerData(ImageData.Data(), ImageData.Size());
	return mCache;
}


}


