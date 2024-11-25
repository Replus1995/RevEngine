#include "Rev/Render/Material/PBRMaterial.h"
#include "Rev/Render/RHI/RHIShaderLibrary.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHICommandList.h"
#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Render/UniformLayout.h"
#include "Rev/Render/RenderUtils.h"
#include "Rev/Render/Resource/TextureResource.h"


namespace Rev
{

FPBRMaterial::FPBRMaterial()
{

}

FPBRMaterial::~FPBRMaterial()
{
}

void FPBRMaterial::Compile()
{
	FRHIShaderCompileOptions FragCompileOps;

	ShaderProgram = FRHIShaderLibrary::Get()->CreateGraphicsProgram("PbrProgram",
		{ "/Engine/Shaders/CommonVS" },
		{ "/Engine/Shaders/ForwardPbrPS", FragCompileOps }
	);
}

void FPBRMaterial::PreDraw(FRHICommandList& RHICmdList)
{
	FMaterial::PreDraw(RHICmdList); //BindProgram
	if(!ShaderProgram)
		return;

	if (!MaterialUB)
		MaterialUB = GDynamicRHI->RHICreateUniformBuffer(sizeof(FPBRMaterialUniform));

	MaterialUB->UpdateSubData(&PBRMaterialParams, sizeof(FPBRMaterialUniform));
	RHICmdList.GetContext()->RHIBindUniformBuffer(UB::Material, MaterialUB.get());

	/*mProgram->SetUniform(UL::LBaseColorFactor, BaseColorFactor);
	mProgram->SetUniform(UL::LMetallic, Metallic);
	mProgram->SetUniform(UL::LRoughness, Roughness);
	mProgram->SetUniform(UL::LNormalScale, NormalScale);
	mProgram->SetUniform(UL::LOcclusionStrength, OcclusionStrength);
	mProgram->SetUniform(UL::LEmissiveFactor, EmissiveFactor);*/

	//FAssetLibrary::GetDefaultWhiteTexture()->GetResource()->Bind(0);
	//FAssetLibrary::GetDefaultNormalTexture()->GetResource()->Bind(1);
	
	if (BaseColorTexture)
		RHICmdList.GetContext()->RHIBindTexture(TB::BaseColorTex, BaseColorTexture->GetTextureRHI(), BaseColorTexture->GetSamplerStateRHI());
	else
		RHICmdList.GetContext()->RHIBindTexture(TB::BaseColorTex, GWhiteTexture->GetTextureRHI(), GWhiteTexture->GetSamplerStateRHI());

	if (MetallicRoughnessTexture)
		RHICmdList.GetContext()->RHIBindTexture(TB::MetallicRoughnessTex, MetallicRoughnessTexture->GetTextureRHI(), MetallicRoughnessTexture->GetSamplerStateRHI());
	else
		RHICmdList.GetContext()->RHIBindTexture(TB::MetallicRoughnessTex, GWhiteTexture->GetTextureRHI(), GWhiteTexture->GetSamplerStateRHI());

	if (NormalTexture)
		RHICmdList.GetContext()->RHIBindTexture(TB::NormalTex, NormalTexture->GetTextureRHI(), NormalTexture->GetSamplerStateRHI());
	else
		RHICmdList.GetContext()->RHIBindTexture(TB::NormalTex, GNormalTexture->GetTextureRHI(), GNormalTexture->GetSamplerStateRHI());

	if (OcclusionTexture)
		RHICmdList.GetContext()->RHIBindTexture(TB::OcclusionTex, OcclusionTexture->GetTextureRHI(), OcclusionTexture->GetSamplerStateRHI());
	else
		RHICmdList.GetContext()->RHIBindTexture(TB::OcclusionTex, GWhiteTexture->TextureRHI.get(), GWhiteTexture->SamplerStateRHI.get());

	if (EmissiveTexture)
		RHICmdList.GetContext()->RHIBindTexture(TB::EmissiveTex, EmissiveTexture->GetTextureRHI(), EmissiveTexture->GetSamplerStateRHI());
	else
		RHICmdList.GetContext()->RHIBindTexture(TB::EmissiveTex, GBlackTexture->TextureRHI.get(), GBlackTexture->SamplerStateRHI.get());
}

}


