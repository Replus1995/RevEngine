#include "BaseRendering.h"

#include "Rev/Render/RenderOptions.h"
#include "Rev/Render/RenderUtils.h"
#include "Rev/Render/RenderProxy/SceneProxy.h"
#include "Rev/Render/RHI/RHICommandList.h"
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHIPipeline.h"

#include "SkyRendering.h"

namespace Rev
{

FFGBasePass::FFGBasePass(FFrameGraph& InGraph, const FFGViewData& InViewData)
{
	FRHITextureDesc DescColor = FRHITextureDesc::Create2D(InViewData.Width, InViewData.Height, PF_R8G8B8A8).SetClearColor(Math::FLinearColor(0, 0, 0, 1)).SetFlags(ETextureCreateFlags::ColorTarget);
	FRHITextureDesc DescDepth = FRHITextureDesc::Create2D(InViewData.Width, InViewData.Height, PF_DepthStencil).SetClearColor(FRHITextureClearColor(1.0, 0)).SetFlags(ETextureCreateFlags::DepthStencilTarget);
	FRHITextureDesc DescColorMS = FRHITextureDesc::Create2D(InViewData.Width, InViewData.Height, PF_R8G8B8A8).SetClearColor(Math::FLinearColor(0, 0, 0, 1)).SetFlags(ETextureCreateFlags::ColorTarget).SetNumSamples(GRenderOptions.GetNumSamples());;
	FRHITextureDesc DescDepthMS = FRHITextureDesc::Create2D(InViewData.Width, InViewData.Height, PF_DepthStencil).SetClearColor(FRHITextureClearColor(1.0, 0)).SetFlags(ETextureCreateFlags::DepthStencilTarget).SetNumSamples(GRenderOptions.GetNumSamples());;
	FFGHandle ColorTex = InGraph.Import<FFGTexture>("Scene Color", DescColor, FFGTexture(DescColor));
	FFGHandle ColorTexMS = InGraph.Import<FFGTexture>("Scene Color MS", DescColorMS, FFGTexture(DescColorMS));
	FFGHandle DepthTex = InGraph.Import<FFGTexture>("Scene Depth", DescDepth, FFGTexture(DescDepth));
	FFGHandle DepthTexMS = InGraph.Import<FFGTexture>("Scene Depth MS", DescDepthMS, FFGTexture(DescDepthMS));

	InGraph.AddPass<FFGBasePass::Data>(
		"Base Pass",
		[&](FFGBuilder& InBuilder, FFGBasePass::Data& InData) {

			/*FFGHandle ColorTex = InBuilder.Create<FFGTexture>("Scene Color", DescColor);
			FFGHandle ColorTexMS = InBuilder.Create<FFGTexture>("Scene Color MS", DescColorMS);
			FFGHandle DepthTex = InBuilder.Create<FFGTexture>("Scene Depth", DescDepth);
			FFGHandle DepthTexMS = InBuilder.Create<FFGTexture>("Scene Depth MS", DescDepthMS);*/


			InData.ColorTex = InBuilder.Write(ColorTex);
			InData.ColorTexMS = InBuilder.Write(ColorTexMS);
			InData.DepthTex = InBuilder.Write(DepthTex);
			InData.DepthTexMS = InBuilder.Write(DepthTexMS);

			InData.SetColorTarget(0, ColorTexMS, ColorTex, RTL_Clear);
			InData.SetDepthStencilTarget(DepthTexMS, DepthTex, RTL_Clear, RTL_Clear);
		},
		[=](const FFGBasePass::Data& InData, FFGPassResources& InResources, FFGContextData& InContextData) {

			auto& RHICmdList = InContextData.RHICmdList;

			FFGUtils::RHIBeginPass(RHICmdList, &InData);

			FRHIGraphicsPipelineStateDesc PipelineStateDesc;
			PipelineStateDesc.VertexInputState = GStaticMeshVertexInputState.VertexInputStateRHI.get();
			PipelineStateDesc.NumSamples = GRenderOptions.GetNumSamples();

			FRHIRasterizerStateDesc RasterizerStateDesc;
			RasterizerStateDesc.CullMode = CM_Back;
			PipelineStateDesc.RasterizerState = FRHIPipelineStateCache::Get()->GetOrCreateRasterizerState(RasterizerStateDesc);

			FRHIDepthStencilStateDesc DepthStencilStateDesc;
			DepthStencilStateDesc.bEnableDepthWrite = true;
			DepthStencilStateDesc.DepthTestFunc = CF_Less;
			PipelineStateDesc.DepthStencilState = FRHIPipelineStateCache::Get()->GetOrCreateDepthStencilState(DepthStencilStateDesc);

			FRHIColorBlendStateDesc ColorBlendStateDesc;
			ColorBlendStateDesc.Attachments[0].bEnableBlend = true;
			PipelineStateDesc.ColorBlendState = FRHIPipelineStateCache::Get()->GetOrCreateColorBlendState(ColorBlendStateDesc);

			RHICmdList.GetContext()->RHISetGraphicsPipelineState(PipelineStateDesc);

			InContextData.SceneProxy->DrawSceneOpaque(RHICmdList);

			FFGUtils::RHIEndPass(RHICmdList, &InData);
		}
	);
}

FFGBlitPass::FFGBlitPass(FFrameGraph& InGraph, const FFGViewData& InViewData)
{
	FFGHandle BackTex = InGraph.Import("Backbuffer", FFGTextureExternal::Desc{}, FFGTextureExternal());
	//const FFGSkyPass::Data& SkyPassData = InGraph.GetPassData<FFGSkyPass::Data>();

	FFGPassData* LastPassData = InGraph.GetLastPassData();
	if(!LastPassData || LastPassData->GetColorTexture(0) == KFGInvalidHandle)
		return;

	InGraph.AddPass<FFGBlitPass::Data>(
		"Blit Pass",
		[&](FFGBuilder& InBuilder, FFGBlitPass::Data& InData) {
			InData.ColorTex = InBuilder.Read(LastPassData->GetColorTexture(0));
			InData.BackTex = InBuilder.Write(BackTex);
		},
		[=](const FFGBlitPass::Data& InData, FFGPassResources& InResources, FFGContextData& InContextData) {

			auto& RHICmdList = InContextData.RHICmdList;

			auto& ColorTex = InResources.Get<FFGTexture>(InData.ColorTex);
			RHICmdList.GetContext()->RHIBlitToBackTexture(ColorTex.GetTextureRHI());
		},
		false
	);
}

}