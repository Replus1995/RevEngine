#include "Rev/Render/RenderProxy/LightProxy.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Math/Maths.h"
#include "Rev/World/Entity.h"
#include "Rev/Render/UniformLayout.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHICommandList.h"
#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Render/RHI/RHIPipeline.h"
#include "Rev/Render/Material/ShadowMapMaterial.h"
#include "Rev/Render/RenderUtils.h"
#include "Rev/Render/Resource/TextureResource.h"

namespace Rev
{

FLightProxy::FLightProxy()
{
}

FLightProxy::~FLightProxy()
{
}

void FLightProxy::Prepare(const Ref<FScene>& Scene)
{
	{
		//Directional lights
		uint32 LightCount = 0;
		auto EntityView = Scene->EntityView<DirectionalLightComponent, TransformComponent>();
		for (const auto& [Entiny, LightComp, TransComp] : EntityView.each())
		{
			if (LightCount >= REV_MAX_DIRECTIONAL_LIGHTS)
				break;
			if (LightComp.Light.GetIntensity() <= 0)
				continue;

			uint32 NumShadowMapLayers = 1;

			auto& Light = DirectionalLightParams.Lights[LightCount];
			Light.Direction = TransComp.Transform.Forward();
			Light.Color = LightComp.Light.GetColor();
			Light.Intensity = LightComp.Light.GetIntensity();

			//shadow
			Light.ShadowMapIndex = LightCount;

			if (Light.ShadowMapCount != NumShadowMapLayers)
			{
				Light.ShadowMapCount = NumShadowMapLayers;
				auto& ShadowData = DirectionalShadowData[LightCount];
				ShadowData.Reset();
				if (NumShadowMapLayers > 0)
				{
					ShadowData.InitRHI(NumShadowMapLayers);
				}
			}

			Light.ProjMats[0] = Math::FMatrix4::Othographic(-10, 10, -10, 10, -10, 10);
			//Light.ProjMats[0] = Math::FMatrix4::Perspective(60, 1, 1, 10000);
			Light.ViewMats[0] = TransComp.Transform.ToMatrix().Inverse();

			LightCount++;
		}
		DirectionalLightParams.Count = LightCount;
	}

	if (!ShadowMapMat)
	{
		ShadowMapMat = CreateRef<FShadowMapMaterial>();
		ShadowMapMat->Compile();
	}
	
}

void FLightProxy::SyncResource(FRHICommandList& RHICmdList)
{
	if (!mLightUB)
		mLightUB = GDynamicRHI->RHICreateUniformBuffer(sizeof(FDirectionalLightUniform));

	mLightUB->UpdateSubData(&DirectionalLightParams, sizeof(FDirectionalLightUniform));
	RHICmdList.GetContext()->RHIBindUniformBuffer(UB::Light, mLightUB.get());

	//Shadow SamplerState
	RHICmdList.GetContext()->RHIBindSamplerState(15, GShadowDepthTexture->GetSamplerStateRHI());

	//Bind DirectionalLight shadow map
	FRHITexture* DirectionalShadowMaps[REV_MAX_DIRECTIONAL_LIGHTS];
	for (uint8 i = 0; i < DirectionalLightParams.Count; i++)
	{
		DirectionalShadowMaps[i] = DirectionalShadowData[i].Texture.get();
	}
	for (uint8 i = DirectionalLightParams.Count; i < REV_MAX_DIRECTIONAL_LIGHTS; i++)
	{
		DirectionalShadowMaps[i] = GShadowDepthTexture->GetTextureRHI();
	}
	RHICmdList.GetContext()->RHIBindTextures(TB::DirectionalShadowMapBegin, DirectionalShadowMaps, 4);
}

FRHITexture* FLightProxy::GetDirectionalShadowMap(uint32 Index)
{
	if (Index >= DirectionalLightParams.Count)
		return nullptr;

	return DirectionalShadowData[Index].Texture.get();
}

void FLightProxy::BeginDirectionalShadowPass(FRHICommandList& RHICmdList, uint32 Index)
{
	if (Index >= DirectionalLightParams.Count || LastShadowRenderData)
		return;

	auto& ShadowData = DirectionalShadowData[Index];
	if (!ShadowData.RenderPass)
		return;

	ShadowData.ViewUniform->UpdateSubData(&DirectionalLightParams.Lights[Index], sizeof(FShadowViewUniform));
	RHICmdList.GetContext()->RHIBindUniformBuffer(UB::ShadowView, ShadowData.ViewUniform.get());
	RHICmdList.GetContext()->RHIBeginRenderPass(ShadowData.RenderPass.get());

	FRHIGraphicsPipelineStateDesc PipelineStateDesc;
	PipelineStateDesc.VertexInputState = GStaticMeshVertexInputState.VertexInputStateRHI.get();

	FRHIRasterizerStateDesc RasterizerStateDesc;
	RasterizerStateDesc.CullMode = CM_Back;
	PipelineStateDesc.RasterizerState = FRHIPipelineStateCache::Get()->GetOrCreateRasterizerState(RasterizerStateDesc);

	FRHIDepthStencilStateDesc DepthStencilStateDesc;
	DepthStencilStateDesc.bEnableDepthWrite = true;
	DepthStencilStateDesc.DepthTestFunc = CF_Less;
	PipelineStateDesc.DepthStencilState = FRHIPipelineStateCache::Get()->GetOrCreateDepthStencilState(DepthStencilStateDesc);

	FRHIColorBlendStateDesc ColorBlendStateDesc;
	ColorBlendStateDesc.Attachments[0].bEnableBlend = false;
	PipelineStateDesc.ColorBlendState = FRHIPipelineStateCache::Get()->GetOrCreateColorBlendState(ColorBlendStateDesc);

	RHICmdList.GetContext()->RHISetGraphicsPipelineState(PipelineStateDesc);


	ShadowMapMat->PreDraw(RHICmdList);

	LastShadowRenderData = &ShadowData;
}

void FLightProxy::EndShadowPass(FRHICommandList& RHICmdList)
{
	if(!LastShadowRenderData)
		return;

	ShadowMapMat->PostDraw(RHICmdList);

	RHICmdList.GetContext()->RHIEndRenderPass();
	LastShadowRenderData = nullptr;
}

void FShadowRenderData::InitRHI(uint32 ShadowMapLayers)
{
	if (!ViewUniform)
		ViewUniform = GDynamicRHI->RHICreateUniformBuffer(sizeof(FShadowViewUniform));

	if (!Texture)
	{
		FRHITextureDesc ShadowMapDesc = FRHITextureDesc::Create2DArray(REV_SHADOWMAP_SIZE, REV_SHADOWMAP_SIZE, ShadowMapLayers, PF_ShadowDepth).SetClearColor(FRHITextureClearColor(1.0, 0)).SetFlags(ETextureCreateFlags::DepthStencilTarget);
		Texture = GDynamicRHI->RHICreateTexture(ShadowMapDesc);
	}

	if (!RenderPass)
	{
		FRHIRenderPassDesc ShadowPassDesc;
		ShadowPassDesc.DepthStencilRenderTarget = { Texture.get(), nullptr, RTL_Clear, RTS_Store, RTL_DontCare, RTS_DontCare };
		ShadowPassDesc.MultiViewCount = ShadowMapLayers;
		RenderPass = GDynamicRHI->RHICreateRenderPass(ShadowPassDesc);
	}

}

void FShadowRenderData::Reset()
{
	RenderPass.reset();
	Texture.reset();
}

}


