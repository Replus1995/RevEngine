#include "Rev/Render/Material/PBRMaterial.h"
#include "Rev/Render/RHI/RHIShaderLibrary.h"
#include "Rev/Render/RHI/RHITexture.h"
#include "Rev/Asset/AssetLibrary.h"

#include "UniformLocation.hpp"

namespace Rev
{

PBRMaterial::PBRMaterial()
	: Material(FRHIShaderLibrary::GetInstance().CreateGraphicsProgram("PBRProgram","PBRBasic"))
{

}

PBRMaterial::~PBRMaterial()
{
}

void PBRMaterial::UploadUniform()
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

	UploadTextureUniform(BaseColorTexture, UL_BaseColorTexture, 1);
	UploadTextureUniform(MetallicRoughnessTexture, UL_MetallicRoughnessTexture, 2);
	UploadTextureUniform(NormalTexture, UL_NormalTexture, 3);
	UploadTextureUniform(OcclusionTexture, UL_OcclusionTexture, 4);
	UploadTextureUniform(EmissiveTexture, UL_EmissiveTexture, 5);
	
}

void PBRMaterial::UploadTextureUniform(const Ref<class Texture>& InTexture, uint16 InLocation, int TexUnit, int TexUnitFallback)
{
	if (InTexture)
	{
		InTexture->GetResource()->Bind(TexUnit);
		mProgram->SetUniform(InLocation, TexUnit);
	}
	else
	{
		mProgram->SetUniform(InLocation, TexUnitFallback);
	}
}

}


