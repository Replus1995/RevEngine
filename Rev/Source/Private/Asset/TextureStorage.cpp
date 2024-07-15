#include "Rev/Asset/TextureStorage.h"
#include "Rev/Render/RHI/RHIResourceFactory.h"

namespace Rev
{

Ref<Texture> FTextureStorage::CreateTexture()
{
	Ref<Texture> Result = CreateRef<Texture>();
	Result->mRHITexture = FRHIResourceFactory::CreateTexture(TextureDesc, SamplerDesc);
	Result->mRHITexture->UpdateData(ImageData.Data(), ImageData.Size());
	return Result;
}

}


