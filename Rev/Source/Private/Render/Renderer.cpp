#include "Rev/Render/Renderer.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Render/RHI/RHICommandList.h"
#include "Rev/Render/RHI/RHIPipeline.h"
#include "Rev/Render/RHI/RHITexture.h"
#include "Rev/Render/RenderUtils.h"
#include "Rev/Render/RenderOptions.h"
#include <fstream>

#include "FrameGraph/BaseRendering.h"
#include "FrameGraph/SkyRendering.h"


namespace Rev
{

FRenderer::FRenderer(FSceneProxy* InSceneProxy)
	: mSceneProxy(InSceneProxy)
{
}

FRenderer::~FRenderer()
{
}

void FRenderer::BeginFrame(FRHICommandList& RHICmdList)
{
	//prepare resource

	uint32 NewFrameWidth = RHICmdList.GetContext()->RHIGetFrameWidth();
	uint32 NewFrameHeight = RHICmdList.GetContext()->RHIGetFrameHeight();

	if (FrameWidth != NewFrameWidth || FrameHeight != NewFrameHeight)
	{
		FrameWidth = NewFrameWidth;
		FrameHeight = NewFrameHeight;

		bDirtyFG = true;
	}

	if (bDirtyFG)
	{
		BuildFrameGraph();
		bDirtyFG = false;
	}

}

void FRenderer::DrawFrame(FRHICommandList& RHICmdList)
{
	mSceneProxy->SyncResource(RHICmdList);

	/*if (mFG)
	{
		mFG->execute(&RHICmdList);
	}*/

	FFGContextData ContextData(RHICmdList, mSceneProxy);
	mGraph.Execute(ContextData);
}

void FRenderer::EndFrame(FRHICommandList& RHICmdList)
{
}

void FRenderer::BuildFrameGraph()
{

	mGraph.Reset();

	FFGViewData ViewData = { FrameWidth,  FrameHeight };
	FFGBasePass BasePass(mGraph, ViewData);
	FFGSkyPass SkyPass(mGraph, ViewData);
	FFGBlitPass BlitPass(mGraph, ViewData);

	mGraph.Compile();

	std::ofstream{ "FrameGraph.dot" } << mGraph;

	/*mFG = CreateScope<FrameGraph>();
	mFGB = CreateScope<FrameGraphBlackboard>();

	struct BasePassData
	{
		FrameGraphResource ColorTex;
		FrameGraphResource DepthTex;
		FrameGraphResource ColorTexMS;
		FrameGraphResource DepthTexMS;
		mutable Ref<FRHIRenderPass> BasePass = nullptr;
	};

	mFGB->add<BasePassData>() = mFG->addCallbackPass<BasePassData>(
		"Base Pass",
		[&](FrameGraph::Builder& builder, BasePassData& data) {

			FRHITextureDesc DescColor = FRHITextureDesc::Create2D(FrameWidth, FrameHeight, PF_R8G8B8A8).SetClearColor(Math::FLinearColor(0, 0, 0, 1)).SetFlags(ETextureCreateFlags::ColorTarget);
			data.ColorTex = builder.create<FFGTexture>("SceneColor", DescColor);
			data.ColorTex = builder.write(data.ColorTex);

			FRHITextureDesc DescDepth = FRHITextureDesc::Create2D(FrameWidth, FrameHeight, PF_DepthStencil).SetClearColor(FRHITextureClearColor(1.0, 0)).SetFlags(ETextureCreateFlags::DepthStencilTarget);
			data.DepthTex = builder.create<FFGTexture>("SceneDepth", DescDepth);
			data.DepthTex = builder.write(data.DepthTex);

			FRHITextureDesc DescColorMS = FRHITextureDesc::Create2D(FrameWidth, FrameHeight, PF_R8G8B8A8).SetClearColor(Math::FLinearColor(0, 0, 0, 1)).SetFlags(ETextureCreateFlags::ColorTarget).SetNumSamples(GRenderOptions.GetNumSamples());;
			data.ColorTexMS = builder.create<FFGTexture>("SceneColorMS", DescColorMS);
			data.ColorTexMS = builder.write(data.ColorTexMS);

			FRHITextureDesc DescDepthMS = FRHITextureDesc::Create2D(FrameWidth, FrameHeight, PF_DepthStencil).SetClearColor(FRHITextureClearColor(1.0, 0)).SetFlags(ETextureCreateFlags::DepthStencilTarget).SetNumSamples(GRenderOptions.GetNumSamples());;
			data.DepthTexMS = builder.create<FFGTexture>("SceneDepthMS", DescDepthMS);
			data.DepthTexMS = builder.write(data.DepthTexMS);
		},
		[=](const BasePassData& data, FrameGraphPassResources& resources, void* ctx) {

			if (!data.BasePass)
			{
				auto& ColorTex = resources.get<FFGTexture>(data.ColorTex);
				auto& DepthTex = resources.get<FFGTexture>(data.DepthTex);
				auto& ColorTexMS = resources.get<FFGTexture>(data.ColorTexMS);
				auto& DepthTexMS = resources.get<FFGTexture>(data.DepthTexMS);

				FRHIRenderPassDesc DescBasePass;
				DescBasePass.ColorRenderTargets[0] = { ColorTexMS.GetTextureRHI(), ColorTex.GetTextureRHI(), -1, 0, RTL_Clear, RTS_Store };
				DescBasePass.NumColorRenderTargets = 1;
				DescBasePass.DepthStencilRenderTarget = { DepthTexMS.GetTextureRHI(), DepthTex.GetTextureRHI(), RTL_Clear, RTS_Store, RTL_DontCare, RTS_DontCare };

				data.BasePass = GDynamicRHI->RHICreateRenderPass(DescBasePass);
			}

			auto& RHICmdList = *static_cast<FRHICommandList*>(ctx);
			RHICmdList.GetContext()->RHIBeginDebugLabel("Base Pass", Math::FLinearColor(0.8f, 0.8f, 0.0f));
			RHICmdList.GetContext()->RHIBeginRenderPass(data.BasePass.get());

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

			mSceneProxy->DrawSceneOpaque(RHICmdList);

			RHICmdList.GetContext()->RHIEndRenderPass();
			RHICmdList.GetContext()->RHIEndDebugLabel();
		}
	);

	const auto& BaseData = mFGB->get<BasePassData>();

	struct SkyPassData
	{
		FrameGraphResource ColorTex;
		FrameGraphResource DepthTex;
		mutable Ref<FRHIRenderPass> SkyPass = nullptr;
	};

	mFGB->add<SkyPassData>() = mFG->addCallbackPass<SkyPassData>(
		"Sky Pass",
		[&](FrameGraph::Builder& builder, SkyPassData& data) {
			data.DepthTex = builder.read(BaseData.DepthTex);

			data.ColorTex = builder.write(BaseData.ColorTex);
		},
		[=](const SkyPassData& data, FrameGraphPassResources& resources, void* ctx) {

			auto& ColorTex = resources.get<FFGTexture>(data.ColorTex);
			auto& DepthTex = resources.get<FFGTexture>(data.DepthTex);

			if (!data.SkyPass)
			{
				FRHIRenderPassDesc SkyPassDesc;
				SkyPassDesc.ColorRenderTargets[0] = { ColorTex.GetTextureRHI(), nullptr, -1, 0, RTL_Load, RTS_Store };
				SkyPassDesc.NumColorRenderTargets = 1;
				SkyPassDesc.DepthStencilRenderTarget = { DepthTex.GetTextureRHI(), nullptr, RTL_Load, RTS_DontCare, RTL_DontCare, RTS_DontCare };

				data.SkyPass = GDynamicRHI->RHICreateRenderPass(SkyPassDesc);
			}

			auto& RHICmdList = *static_cast<FRHICommandList*>(ctx);
			RHICmdList.GetContext()->RHIBeginDebugLabel("Sky Pass", Math::FLinearColor(0.6f, 0.6f, 0.9f));
			RHICmdList.GetContext()->RHIBeginRenderPass(data.SkyPass.get());

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

			mSceneProxy->DrawSkybox(RHICmdList);

			RHICmdList.GetContext()->RHIEndRenderPass();
			RHICmdList.GetContext()->RHIEndDebugLabel();
		}
	);

	const auto& SkyData = mFGB->get<SkyPassData>();

	auto backbufferId = mFG->import("Backbuffer", {}, FFGTexture());

	struct BlitPassData {
		FrameGraphResource ColorTex;
		FrameGraphResource BackTex;
	};

	mFG->addCallbackPass<BlitPassData>(
		"Blit Pass", 
		[&](FrameGraph::Builder& builder, BlitPassData& data) {
			data.ColorTex = builder.read(SkyData.ColorTex);
			data.BackTex = builder.write(backbufferId);
		}, 
		[=](const BlitPassData& data, FrameGraphPassResources& resources, void* ctx) {

			auto& ColorTex = resources.get<FFGTexture>(data.ColorTex);
			auto& RHICmdList = *static_cast<FRHICommandList*>(ctx);
			RHICmdList.GetContext()->RHIBlitToBackTexture(ColorTex.GetTextureRHI());
		}
	);


	mFG->compile();*/

	//std::ofstream{ "FrameGraph.dot" } << *mFG;
}

}