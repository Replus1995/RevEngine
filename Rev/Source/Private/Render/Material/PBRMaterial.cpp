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

	ShaderProgram = FRHIShaderLibrary::GetInstance().CreateGraphicsProgram("PbrProgram",
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
	RHICmdList.GetContext()->RHIBindUniformBuffer(UL::BMaterial, MaterialUB.get());

	/*mProgram->SetUniform(UL::LBaseColorFactor, BaseColorFactor);
	mProgram->SetUniform(UL::LMetallic, Metallic);
	mProgram->SetUniform(UL::LRoughness, Roughness);
	mProgram->SetUniform(UL::LNormalScale, NormalScale);
	mProgram->SetUniform(UL::LOcclusionStrength, OcclusionStrength);
	mProgram->SetUniform(UL::LEmissiveFactor, EmissiveFactor);*/

	//FAssetLibrary::GetDefaultWhiteTexture()->GetResource()->Bind(0);
	//FAssetLibrary::GetDefaultNormalTexture()->GetResource()->Bind(1);
	
	if (BaseColorTexture)
		RHICmdList.GetContext()->RHIBindTexture(UL::SBaseColorTex, BaseColorTexture->GetTextureRHI(), BaseColorTexture->GetSamplerStateRHI());
	else
		RHICmdList.GetContext()->RHIBindTexture(UL::SBaseColorTex, GWhiteTexture->GetTextureRHI(), GWhiteTexture->GetSamplerStateRHI());

	if (MetallicRoughnessTexture)
		RHICmdList.GetContext()->RHIBindTexture(UL::SMetallicRoughnessTex, MetallicRoughnessTexture->GetTextureRHI(), MetallicRoughnessTexture->GetSamplerStateRHI());
	else
		RHICmdList.GetContext()->RHIBindTexture(UL::SMetallicRoughnessTex, GWhiteTexture->GetTextureRHI(), GWhiteTexture->GetSamplerStateRHI());

	if (NormalTexture)
		RHICmdList.GetContext()->RHIBindTexture(UL::SNormalTex, NormalTexture->GetTextureRHI(), NormalTexture->GetSamplerStateRHI());
	else
		RHICmdList.GetContext()->RHIBindTexture(UL::SNormalTex, GNormalTexture->GetTextureRHI(), GNormalTexture->GetSamplerStateRHI());

	if (OcclusionTexture)
		RHICmdList.GetContext()->RHIBindTexture(UL::SOcclusionTex, OcclusionTexture->GetTextureRHI(), OcclusionTexture->GetSamplerStateRHI());
	else
		RHICmdList.GetContext()->RHIBindTexture(UL::SOcclusionTex, GWhiteTexture->TextureRHI.get(), GWhiteTexture->SamplerStateRHI.get());

	if (EmissiveTexture)
		RHICmdList.GetContext()->RHIBindTexture(UL::SEmissiveTex, EmissiveTexture->GetTextureRHI(), EmissiveTexture->GetSamplerStateRHI());
	else
		RHICmdList.GetContext()->RHIBindTexture(UL::SEmissiveTex, GBlackTexture->TextureRHI.get(), GBlackTexture->SamplerStateRHI.get());
}

}


