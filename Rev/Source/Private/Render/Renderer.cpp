#include "Rev/Render/Renderer.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Render/RHI/RHICommandList.h"
#include "Rev/Render/RHI/RHIPipeline.h"
#include "Rev/Render/RHI/RHITexture.h"
#include "Rev/Render/RenderUtils.h"
#include "Rev/Render/RenderOptions.h"
#include <fstream>


namespace Rev
{

FRenderer::FRenderer(FSceneProxy* InSceneProxy)
	: mSceneProxy(InSceneProxy)
{
}

FRenderer::~FRenderer()
{
}

void FRenderer::BeginFrame()
{
	//prepare resource

	/*if (!mBaseColorTex)
	{
		FRHITextureDesc Desc = FRHITextureDesc::Create2D(mSceneProxy->GetFrameWidth(), mSceneProxy->GetFrameHeight(), PF_R8G8B8A8).SetClearColor(Math::FLinearColor(0, 0, 0, 1)).SetFlags(ETextureCreateFlags::ColorTarget);
		mBaseColorTex = GDynamicRHI->RHICreateTexture(Desc);
	}
	if (!mBaseColorResolveTex)
	{
		FRHITextureDesc Desc = FRHITextureDesc::Create2D(mSceneProxy->GetFrameWidth(), mSceneProxy->GetFrameHeight(), PF_R8G8B8A8).SetClearColor(Math::FLinearColor(0, 0, 0, 1)).SetFlags(ETextureCreateFlags::ColorTarget).SetNumSamples(GRenderOptions.GetNumSamples());
		mBaseColorResolveTex = GDynamicRHI->RHICreateTexture(Desc);
	}
	if (!mBaseDepthTex)
	{
		FRHITextureDesc Desc = FRHITextureDesc::Create2D(mSceneProxy->GetFrameWidth(), mSceneProxy->GetFrameHeight(), PF_DepthStencil).SetClearColor(FRHITextureClearColor(1.0, 0)).SetFlags(ETextureCreateFlags::DepthStencilTarget);
		mBaseDepthTex = GDynamicRHI->RHICreateTexture(Desc);
	}
	if (!mBaseDepthResolveTex)
	{
		FRHITextureDesc Desc = FRHITextureDesc::Create2D(mSceneProxy->GetFrameWidth(), mSceneProxy->GetFrameHeight(), PF_DepthStencil).SetClearColor(FRHITextureClearColor(1.0, 0)).SetFlags(ETextureCreateFlags::DepthStencilTarget).SetNumSamples(GRenderOptions.GetNumSamples());
		mBaseDepthResolveTex = GDynamicRHI->RHICreateTexture(Desc);
	}
	if (!mBasePass)
	{

		FRHIRenderPassDesc BasePassDesc;
		BasePassDesc.ColorRenderTargets[0] = { mBaseColorResolveTex.get(), mBaseColorTex.get(), -1, 0, ALO_Clear, ASO_Store};
		BasePassDesc.NumColorRenderTargets = 1;
		BasePassDesc.DepthStencilRenderTarget  = { mBaseDepthResolveTex.get(), mBaseDepthTex.get(), ALO_Clear, ASO_Store, ALO_DontCare, ASO_DontCare};
		mBasePass = GDynamicRHI->RHICreateRenderPass(BasePassDesc);
	}
	
	if (!mSkyPass)
	{
		FRHIRenderPassDesc SkyPassDesc;
		SkyPassDesc.ColorRenderTargets[0] = { mBaseColorTex.get(), nullptr, -1, 0, ALO_Load, ASO_Store };
		SkyPassDesc.NumColorRenderTargets = 1;
		SkyPassDesc.DepthStencilRenderTarget = { mBaseDepthTex.get(), nullptr, ALO_Load, ASO_DontCare, ALO_DontCare, ASO_DontCare };
		mSkyPass = GDynamicRHI->RHICreateRenderPass(SkyPassDesc);
	}*/

	if (FrameWidth != mSceneProxy->GetFrameWidth() || FrameHeight != mSceneProxy->GetFrameHeight())
	{
		FrameWidth = mSceneProxy->GetFrameWidth();
		FrameHeight = mSceneProxy->GetFrameHeight();
		//GDynamicRHI->RHIResizeTexture(mBaseColorTex.get(), FrameWidth, FrameHeight, 1);
		//GDynamicRHI->RHIResizeTexture(mBaseDepthTex.get(), FrameWidth, FrameHeight, 1);
		//mBasePass->MarkFramebufferDirty();

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

	/*{
		RHICmdList.GetContext()->RHIBeginDebugLabel("Base Pass", Math::FLinearColor(0.8f, 0.8f, 0.0f));
		RHICmdList.GetContext()->RHIBeginRenderPass(mBasePass.get());

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

	{
		RHICmdList.GetContext()->RHIBeginDebugLabel("Sky Pass", Math::FLinearColor(0.6f, 0.6f, 0.9f));
		RHICmdList.GetContext()->RHIBeginRenderPass(mSkyPass.get());

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
	
	RHICmdList.GetContext()->RHIBlitToBackTexture(mBaseColorTex.get());*/

	if (mFG)
	{
		mFG->execute(&RHICmdList);
	}


}

void FRenderer::EndFrame()
{
}

void FRenderer::BuildFrameGraph()
{

	mFG = CreateScope<FrameGraph>();
	mFGB = CreateScope<FrameGraphBlackboard>();
	//mBasePass.reset();
	//mSkyPass.reset();

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

			FRHITextureDesc DescColor = FRHITextureDesc::Create2D(mSceneProxy->GetFrameWidth(), mSceneProxy->GetFrameHeight(), PF_R8G8B8A8).SetClearColor(Math::FLinearColor(0, 0, 0, 1)).SetFlags(ETextureCreateFlags::ColorTarget);
			data.ColorTex = builder.create<FGTexture>("SceneColor", DescColor);
			data.ColorTex = builder.write(data.ColorTex);

			FRHITextureDesc DescDepth = FRHITextureDesc::Create2D(mSceneProxy->GetFrameWidth(), mSceneProxy->GetFrameHeight(), PF_DepthStencil).SetClearColor(FRHITextureClearColor(1.0, 0)).SetFlags(ETextureCreateFlags::DepthStencilTarget);
			data.DepthTex = builder.create<FGTexture>("SceneDepth", DescDepth);
			data.DepthTex = builder.write(data.DepthTex);

			FRHITextureDesc DescColorMS = FRHITextureDesc::Create2D(mSceneProxy->GetFrameWidth(), mSceneProxy->GetFrameHeight(), PF_R8G8B8A8).SetClearColor(Math::FLinearColor(0, 0, 0, 1)).SetFlags(ETextureCreateFlags::ColorTarget).SetNumSamples(GRenderOptions.GetNumSamples());;
			data.ColorTexMS = builder.create<FGTexture>("SceneColorMS", DescColorMS);
			data.ColorTexMS = builder.write(data.ColorTexMS);

			FRHITextureDesc DescDepthMS = FRHITextureDesc::Create2D(mSceneProxy->GetFrameWidth(), mSceneProxy->GetFrameHeight(), PF_DepthStencil).SetClearColor(FRHITextureClearColor(1.0, 0)).SetFlags(ETextureCreateFlags::DepthStencilTarget).SetNumSamples(GRenderOptions.GetNumSamples());;
			data.DepthTexMS = builder.create<FGTexture>("SceneDepthMS", DescDepthMS);
			data.DepthTexMS = builder.write(data.DepthTexMS);
		},
		[=](const BasePassData& data, FrameGraphPassResources& resources, void* ctx) {

			if (!data.BasePass)
			{
				auto& ColorTex = resources.get<FGTexture>(data.ColorTex);
				auto& DepthTex = resources.get<FGTexture>(data.DepthTex);
				auto& ColorTexMS = resources.get<FGTexture>(data.ColorTexMS);
				auto& DepthTexMS = resources.get<FGTexture>(data.DepthTexMS);

				FRHIRenderPassDesc DescBasePass;
				DescBasePass.ColorRenderTargets[0] = { ColorTexMS.TextureRHI.get(), ColorTex.TextureRHI.get(), -1, 0, ALO_Clear, ASO_Store };
				DescBasePass.NumColorRenderTargets = 1;
				DescBasePass.DepthStencilRenderTarget = { DepthTexMS.TextureRHI.get(), DepthTex.TextureRHI.get(), ALO_Clear, ASO_Store, ALO_DontCare, ASO_DontCare };

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

			auto& ColorTex = resources.get<FGTexture>(data.ColorTex);
			auto& DepthTex = resources.get<FGTexture>(data.DepthTex);

			if (!data.SkyPass)
			{
				FRHIRenderPassDesc SkyPassDesc;
				SkyPassDesc.ColorRenderTargets[0] = { ColorTex.TextureRHI.get(), nullptr, -1, 0, ALO_Load, ASO_Store };
				SkyPassDesc.NumColorRenderTargets = 1;
				SkyPassDesc.DepthStencilRenderTarget = { DepthTex.TextureRHI.get(), nullptr, ALO_Load, ASO_DontCare, ALO_DontCare, ASO_DontCare };

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

			//RHICmdList.GetContext()->RHIBlitToBackTexture(ColorTex.TextureRHI.get());
		}
	);

	const auto& SkyData = mFGB->get<SkyPassData>();

	auto backbufferId = mFG->import("Backbuffer", {}, FGTexture());

	struct BlitPassData {
		FrameGraphResource ColorTex;
		FrameGraphResource BackTex;
	};

	mFG->addCallbackPass<BlitPassData>(
		"Blit pass", 
		[&](FrameGraph::Builder& builder, BlitPassData& data) {
			data.ColorTex = builder.read(SkyData.ColorTex);
			data.BackTex = builder.write(backbufferId);
		}, 
		[=](const BlitPassData& data, FrameGraphPassResources& resources, void* ctx) {

			auto& ColorTex = resources.get<FGTexture>(data.ColorTex);
			auto& RHICmdList = *static_cast<FRHICommandList*>(ctx);
			RHICmdList.GetContext()->RHIBlitToBackTexture(ColorTex.TextureRHI.get());
		}
	);


	mFG->compile();


	//std::ofstream{ "FrameGraph.dot" } << *mFG;
}

}