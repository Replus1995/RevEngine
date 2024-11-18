#include "SkyRendering.h"

#include "Rev/Render/RenderUtils.h"
#include "Rev/Render/RenderProxy/SceneProxy.h"
#include "Rev/Render/RHI/RHICommandList.h"
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHIPipeline.h"

#include "BaseRendering.h"


namespace Rev
{

FFGSkyPass::FFGSkyPass(FFrameGraph& InGraph, const FFGViewData& InViewData)
{
	FFGBasePass::Data& BasePassData = InGraph.GetPassData<FFGBasePass::Data>();

	InGraph.AddPass<FFGSkyPass::Data>(
		"Sky Pass",
		[&](FFGBuilder& InBuilder, FFGSkyPass::Data& InData) {
			InData.ColorTex = InBuilder.Read(BasePassData.ColorTex);
			InData.DepthTex = InBuilder.Write(BasePassData.DepthTex);

			InData.SetColorTarget(0, InData.ColorTex, KInvalidHandle, RTL_Load);
			InData.SetDepthStencilTarget(InData.DepthTex, KInvalidHandle, RTL_Load, RTL_DontCare, RTS_DontCare, RTS_DontCare);
		},
		[=](const FFGSkyPass::Data& InData, FFGPassResources& InResources, FFGContextData& InContextData) {

			auto& RHICmdList = InContextData.RHICmdList;

			FRHIGraphicsPipelineStateDesc PipelineStateDesc;
			PipelineStateDesc.VertexInputState = GTileVertexInputState.VertexInputStateRHI.get();

			FRHIRasterizerStateDesc RasterizerStateDesc;
			RasterizerStateDesc.CullMode = CM_None;
			PipelineStateDesc.RasterizerState = FRHIPipelineStateCache::Get()->GetOrCreateRasterizerState(RasterizerStateDesc);

			FRHIDepthStencilStateDesc DepthStencilStateDesc;
			DepthStencilStateDesc.bEnableDepthWrite = false;
			DepthStencilStateDesc.DepthTestFunc = CF_LessEqual;
			PipelineStateDesc.DepthStencilState = FRHIPipelineStateCache::Get()->GetOrCreateDepthStencilState(DepthStencilStateDesc);

			FRHIColorBlendStateDesc ColorBlendStateDesc;
			ColorBlendStateDesc.Attachments[0].bEnableBlend = false;
			PipelineStateDesc.ColorBlendState = FRHIPipelineStateCache::Get()->GetOrCreateColorBlendState(ColorBlendStateDesc);

			RHICmdList.GetContext()->RHISetGraphicsPipelineState(PipelineStateDesc);

			InContextData.SceneProxy->DrawSkybox(RHICmdList);
		}
	);
}

}