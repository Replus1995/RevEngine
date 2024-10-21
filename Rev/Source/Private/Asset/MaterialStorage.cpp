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

	Result->Params.BaseColorFactor = BaseColorFactor;
	Result->Params.Metallic = Metallic;
	Result->Params.Roughness = Roughness;
	Result->Params.NormalScale = NormalScale;
	Result->Params.OcclusionStrength = OcclusionStrength;
	Result->Params.EmissiveFactor = EmissiveFactor;

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


