#include "Rev/Render/Material/PBRMaterial.h"
#include "Rev/Render/RHI/RHIShaderLibrary.h"
#include "Rev/Render/UniformLayout.h"



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

	mProgram = FRHIShaderLibrary::GetInstance().CreateGraphicsProgram("PbrProgram",
		{ "/Engine/Shaders/CommonVS" },
		{ "/Engine/Shaders/PBR/ForwardFS", FragCompileOps }
	);
}

void PBRMaterial::PreDraw(FRHICommandList& RHICmdList)
{
	SurfaceMaterial::PreDraw(RHICmdList); //BindProgram
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
	
	/*if (BaseColorTexture)
		RenderCmd::BindTexture(BaseColorTexture->GetResource(), UL::SBaseColorTex);
	if (MetallicRoughnessTexture)
		RenderCmd::BindTexture(MetallicRoughnessTexture->GetResource(), UL::SMetallicRoughnessTex);
	if (NormalTexture)
		RenderCmd::BindTexture(NormalTexture->GetResource(), UL::SNormalTex);
	if (OcclusionTexture)
		RenderCmd::BindTexture(OcclusionTexture->GetResource(), UL::SOcclusionTex);
	if (EmissiveTexture)
		RenderCmd::BindTexture(EmissiveTexture->GetResource(), UL::SEmissiveTex);*/
}

}


