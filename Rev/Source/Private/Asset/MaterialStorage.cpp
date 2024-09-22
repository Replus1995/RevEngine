#include "Rev/Asset/MaterialStorage.h"
#include "Rev/Asset/TextureStorage.h"
#include "Rev/Render/Material/PBRMaterial.h"

namespace Rev
{

Ref<SurfaceMaterial> FPBRMaterialStorage::CreateMaterial()
{
	if(mCache)
		return mCache;

	Ref<PBRMaterial> Result = CreateRef<PBRMaterial>();
	Result->BlendMode = BlendMode;
	Result->ShadingModel = ShadingModel;
	Result->MaskClip = MaskClip;
	Result->TwoSided = TwoSided;

	Result->BaseColorFactor = BaseColorFactor;
	Result->Metallic = Metallic;
	Result->Roughness = Roughness;
	Result->NormalScale = NormalScale;
	Result->OcclusionStrength = OcclusionStrength;
	Result->EmissiveFactor = EmissiveFactor;

	if(BaseColorTexture)
		Result->BaseColorTexture = BaseColorTexture->CreateTexture(true);
	if (MetallicRoughnessTexture)
		Result->MetallicRoughnessTexture = MetallicRoughnessTexture->CreateTexture();
	if (NormalTexture)
		Result->NormalTexture = NormalTexture->CreateTexture();
	if (OcclusionTexture)
		Result->OcclusionTexture = OcclusionTexture->CreateTexture();
	if (EmissiveTexture)
		Result->EmissiveTexture = EmissiveTexture->CreateTexture(true);

	Result->Compile();

	mCache = Result;
	return mCache;
}

}


