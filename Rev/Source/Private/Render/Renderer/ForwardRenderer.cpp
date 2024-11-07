#include "Rev/Render/Renderer/ForwardRenderer.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Render/RHI/RHICommandList.h"
#include "Rev/Render/RHI/RHIPipeline.h"
#include "Rev/Render/RHI/RHITexture.h"
#include "Rev/Render/RenderProxy/SceneProxy.h"
#include "Rev/Render/RenderUtils.h"
#include "Rev/Render/RenderOptions.h"


namespace Rev
{

FForwardRenderer::FForwardRenderer(FSceneProxy* InSceneProxy)
	: FRenderer(InSceneProxy)
{
}

FForwardRenderer::~FForwardRenderer()
{
}

void FForwardRenderer::BeginFrame()
{
	//prepare resource

	if (!mBaseColorTex)
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
	}

	if (FrameWidth != mSceneProxy->GetFrameWidth() || FrameHeight != mSceneProxy->GetFrameHeight())
	{
		FrameWidth = mSceneProxy->GetFrameWidth();
		FrameHeight = mSceneProxy->GetFrameHeight();
		GDynamicRHI->RHIResizeTexture(mBaseColorTex.get(), FrameWidth, FrameHeight, 1);
		GDynamicRHI->RHIResizeTexture(mBaseDepthTex.get(), FrameWidth, FrameHeight, 1);
		mBasePass->MarkFramebufferDirty();
	}

	//else
	//{
	//	mLinearScreenTarget->ResizeTargets(mContext->Width, mContext->Height);
	//}
	
}

void FForwardRenderer::DrawFrame(FRHICommandList& RHICmdList)
{
	// (vulkan)
	// begin cmd buffer 
		// begin render pass
			// begin sub pass
			// end sub pass
		// end render pass
	// end cmd buffer


	mSceneProxy->SyncResource(RHICmdList);

	{
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
	
	RHICmdList.GetContext()->RHIBlitToBackTexture(mBaseColorTex.get());


	/*
	RenderCmd::SetCullFaceMode(CFM_Back);

	mGammaCorrectPass.BeginPass();
	mGammaCorrectPass.RunPass();
	mGammaCorrectPass.EndPass();
	*/

}

void FForwardRenderer::EndFrame()
{
}

}