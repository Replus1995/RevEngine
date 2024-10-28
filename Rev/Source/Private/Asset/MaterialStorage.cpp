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

	Result->PBRMaterialParams.BaseColorFactor = BaseColorFactor;
	Result->PBRMaterialParams.Metallic = Metallic;
	Result->PBRMaterialParams.Roughness = Roughness;
	Result->PBRMaterialParams.NormalScale = NormalScale;
	Result->PBRMaterialParams.OcclusionStrength = OcclusionStrength;
	Result->PBRMaterialParams.EmissiveFactor = EmissiveFactor;

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


