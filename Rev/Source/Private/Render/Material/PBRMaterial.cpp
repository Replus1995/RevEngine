#include "Rev/Render/Material/PBRMaterial.h"
#include "Rev/Render/RHI/RHIShaderLibrary.h"
#include "Rev/Render/RHI/RHITexture.h"
#include "Rev/Asset/AssetLibrary.h"

#include "Private/Render/UniformLocation.h"

namespace Rev
{

PBRMaterial::PBRMaterial()
{

}

PBRMaterial::~PBRMaterial()
{
}

void PBRMaterial::Compile()
{
	mProgram = FRHIShaderLibrary::GetInstance().CreateGraphicsProgram(
		"PbrProgram",
		{ "/Engine/Shaders/CommonVS" },
		{ "/Engine/Shaders/PBR/ForwardFS" }
	);
}

void PBRMaterial::SyncUniform()
{
	if(!mProgram)
		return;

	mProgram->SetUniform(UL_BaseColorFactor, BaseColorFactor);
	mProgram->SetUniform(UL_Metallic, Metallic);
	mProgram->SetUniform(UL_Roughness, Roughness);
	mProgram->SetUniform(UL_NormalScale, NormalScale);
	mProgram->SetUniform(UL_OcclusionStrength, OcclusionStrength);
	mProgram->SetUniform(UL_EmissiveFactor, EmissiveFactor);

	FAssetLibrary::GetDefaultWhiteTexture()->GetResource()->Bind(0);
	FAssetLibrary::GetDefaultNormalTexture()->GetResource()->Bind(1);

	SyncTextureUniform(BaseColorTexture, UL_BaseColorTexture, 2);
	SyncTextureUniform(MetallicRoughnessTexture, UL_MetallicRoughnessTexture, 3);
	SyncTextureUniform(NormalTexture, UL_NormalTexture, 4, 1);
	SyncTextureUniform(OcclusionTexture, UL_OcclusionTexture, 5);
	SyncTextureUniform(EmissiveTexture, UL_EmissiveTexture, 6);
	
}

}


