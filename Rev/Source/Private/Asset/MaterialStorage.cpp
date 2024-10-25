#include "Rev/Asset/MaterialStorage.h"
#include "Rev/Asset/TextureStorage.h"
#include "Rev/Render/Material/PBRMaterial.h"

namespace Rev
{

Ref<FMaterial> FPBRMaterialStorage::CreateMaterial()
{
	if(mCache)
		return mCache;

	Ref<FPBRMaterial> Result = CreateRef<FPBRMaterial>();
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
		Result->BaseColorTexture = BaseColorTexture;
	if (MetallicRoughnessTexture)
		Result->MetallicRoughnessTexture = MetallicRoughnessTexture;
	if (NormalTexture)
		Result->NormalTexture = NormalTexture;
	if (OcclusionTexture)
		Result->OcclusionTexture = OcclusionTexture;
	if (EmissiveTexture)
		Result->EmissiveTexture = EmissiveTexture;

	Result->Compile();

	mCache = Result;
	return mCache;
}

}


