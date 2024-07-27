#include "Rev/Render/Material/PBRMaterial.h"
#include "Rev/Render/RHI/RHIShaderLibrary.h"
#include "Rev/Render/RHI/RHITexture.h"
#include "Rev/Asset/AssetLibrary.h"

//Private
#include "Render/UniformLocation.h"

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
	FRHIShaderCompileOptions FragCompileOps;
	if(BaseColorTexture)
		FragCompileOps.AddMacro(SCM_USE_BASECOLOR_TEX);
	if (MetallicRoughnessTexture)
		FragCompileOps.AddMacro(SCM_USE_METALLICROUGHNESS_TEX);
	if (NormalTexture)
		FragCompileOps.AddMacro(SCM_USE_NORMAL_TEX);
	if (OcclusionTexture)
		FragCompileOps.AddMacro(SCM_USE_OCCLUSION_TEX);
	if (EmissiveTexture)
		FragCompileOps.AddMacro(SCM_USE_EMISSIVE_TEX);


	mProgram = FRHIShaderLibrary::GetInstance().CreateGraphicsProgram(
		"PbrProgram",
		{ "/Engine/Shaders/CommonVS" },
		{ "/Engine/Shaders/PBR/ForwardFS", FragCompileOps }
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

	//FAssetLibrary::GetDefaultWhiteTexture()->GetResource()->Bind(0);
	//FAssetLibrary::GetDefaultNormalTexture()->GetResource()->Bind(1);
	
	if (BaseColorTexture)
		mProgram->SetUniform(UL_BaseColorTexture, BaseColorTexture->GetResource());
	if (MetallicRoughnessTexture)
		mProgram->SetUniform(UL_MetallicRoughnessTexture, MetallicRoughnessTexture->GetResource());
	if (NormalTexture)
		mProgram->SetUniform(UL_NormalTexture, NormalTexture->GetResource());
	if (OcclusionTexture)
		mProgram->SetUniform(UL_OcclusionTexture, OcclusionTexture->GetResource());
	if (EmissiveTexture)
		mProgram->SetUniform(UL_EmissiveTexture, EmissiveTexture->GetResource());
}

}


