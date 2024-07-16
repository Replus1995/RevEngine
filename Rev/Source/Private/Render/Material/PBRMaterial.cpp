#include "Rev/Render/Material/PBRMaterial.h"
#include "Rev/Render/RHI/RHIShaderLibrary.h"
#include "Rev/Render/RHI/RHITexture.h"
#include "Rev/Asset/AssetLibrary.h"

#include "UniformLocation.hpp"

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
	mProgram = FRHIShaderLibrary::GetInstance().CreateGraphicsProgram("PBRProgram", "PBRBasic");
}

void PBRMaterial::SyncUniform()
{
	if(!mProgram)
		return;

	mProgram->SetUniform(UL_BaseColor, BaseColor);
	mProgram->SetUniform(UL_Metallic, Metallic);
	mProgram->SetUniform(UL_Roughness, Roughness);
	mProgram->SetUniform(UL_NormalScale, NormalScale);
	mProgram->SetUniform(UL_OcclusionStrength, OcclusionStrength);
	mProgram->SetUniform(UL_EmissiveColor, EmissiveColor);


	FAssetLibrary::GetWhiteTexture()->GetResource()->Bind(0);

	SyncTextureUniform(BaseColorTexture, UL_BaseColorTexture, 1);
	SyncTextureUniform(MetallicRoughnessTexture, UL_MetallicRoughnessTexture, 2);
	SyncTextureUniform(NormalTexture, UL_NormalTexture, 3);
	SyncTextureUniform(OcclusionTexture, UL_OcclusionTexture, 4);
	SyncTextureUniform(EmissiveTexture, UL_EmissiveTexture, 5);
	
}

}


