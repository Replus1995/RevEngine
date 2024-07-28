#include "Rev/Render/Material/PBRMaterial.h"
#include "Rev/Render/RHI/RHIShaderLibrary.h"
#include "Rev/Render/RHI/RHITexture.h"
#include "Rev/Render/UniformLayout.h"
#include "Rev/Asset/AssetLibrary.h"


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

	mProgram->SetUniform(UL::LBaseColorFactor, BaseColorFactor);
	mProgram->SetUniform(UL::LMetallic, Metallic);
	mProgram->SetUniform(UL::LRoughness, Roughness);
	mProgram->SetUniform(UL::LNormalScale, NormalScale);
	mProgram->SetUniform(UL::LOcclusionStrength, OcclusionStrength);
	mProgram->SetUniform(UL::LEmissiveFactor, EmissiveFactor);

	//FAssetLibrary::GetDefaultWhiteTexture()->GetResource()->Bind(0);
	//FAssetLibrary::GetDefaultNormalTexture()->GetResource()->Bind(1);
	
	if (BaseColorTexture)
		BaseColorTexture->GetResource()->Bind(UL::SBaseColorTex);
	if (MetallicRoughnessTexture)
		MetallicRoughnessTexture->GetResource()->Bind(UL::SMetallicRoughnessTex);
	if (NormalTexture)
		NormalTexture->GetResource()->Bind(UL::SNormalTex);
	if (OcclusionTexture)
		OcclusionTexture->GetResource()->Bind(UL::SOcclusionTex);
	if (EmissiveTexture)
		EmissiveTexture->GetResource()->Bind(UL::SEmissiveTex);
}

}


