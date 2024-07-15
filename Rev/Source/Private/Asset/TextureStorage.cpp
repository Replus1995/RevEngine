#include "Rev/Asset/TextureStorage.h"
#include "Rev/Render/RHI/RHIResourceFactory.h"

namespace Rev
{

Ref<Texture> FTextureStorage::CreateTexture()
{
	if(mCache)
		return mCache;
	mCache = CreateRef<Texture>(FRHIResourceFactory::CreateTexture(TextureDesc, SamplerDesc));
	mCache->GetResource()->UpdateData(ImageData.Data(), ImageData.Size());
	return mCache;
}

}


