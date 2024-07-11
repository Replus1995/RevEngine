#include "OpenGLTexture.h"
#include "Rev/Core/Assert.h"

#include "OpenGLSampler.h"
#include "OpenGLTexture2D.h"

namespace Rev
{

void FOpenGLTexture::SetSampler(const Ref<FRHISampler>& InSampler)
{
	if (!InSampler) return;
	if (!mSamplerDescCache || *mSamplerDescCache != InSampler->GetDesc())
	{
		if(!mSamplerDescCache)
			mSamplerDescCache = CreateScope<FSamplerDesc>();
		*mSamplerDescCache = InSampler->GetDesc();

		FOpenGLSampler* pSampler = static_cast<FOpenGLSampler*>(InSampler.get());
		pSampler->UpdateSampleState(mHandle, mDesc.NumMips > 1);
	}
}

Ref<FOpenGLTexture> FOpenGLTexture::Create(const FTextureDesc& InDesc)
{
	switch (InDesc.Dimension)
	{
	case ETextureDimension::Texture2D:
		return CreateRef<FOpenGLTexture2D>(InDesc);
	case ETextureDimension::Texture2DArray:
	case ETextureDimension::Texture3D:
	case ETextureDimension::TextureCube:
	case ETextureDimension::TextureCubeArray:
	default:
		break;
	}
	RE_CORE_ASSERT(false, "Unsupported Texture Dimension!");
	return nullptr;
}

}


