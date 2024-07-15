#include "Rev/Asset/MaterialStorage.h"
#include "Rev/Asset/TextureStorage.h"
#include "Rev/Render/Material/PBRMaterial.h"

namespace Rev
{

Ref<Material> FPBRMaterialStorage::CreateMaterial()
{
	if(mCache)
		return mCache;

	Ref<PBRMaterial> Result = CreateRef<PBRMaterial>();
	Result->Domain = Domain;
	Result->BlendMode = BlendMode;
	Result->ShadingModel = ShadingModel;
	Result->MaskClip = MaskClip;
	Result->TwoSided = TwoSided;

	Result->BaseColor = BaseColor;
	Result->BaseColorTexture = BaseColorTexture->CreateTexture();
	Result->Metallic = Metallic;
	Result->Roughness = Roughness;
	Result->MetallicRoughnessTexture = MetallicRoughnessTexture->CreateTexture();
	Result->NormalScale = NormalScale;
	Result->NormalTexture = NormalTexture->CreateTexture();
	Result->OcclusionStrength = OcclusionStrength;
	Result->OcclusionTexture = OcclusionTexture->CreateTexture();
	Result->EmissiveColor = EmissiveColor;
	Result->EmissiveTexture = EmissiveTexture->CreateTexture();

	mCache = Result;
	return mCache;
}

}


