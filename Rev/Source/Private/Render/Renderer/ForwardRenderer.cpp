#include "Rev/Render/Renderer/ForwardRenderer.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Render/RHI/RHICommandList.h"
#include "Rev/Render/RHI/RHIPipeline.h"
#include "Rev/Render/RHI/RHITexture.h"
#include "Rev/Render/RenderProxy/SceneProxy.h"
#include "Rev/Render/RenderUtils.h"

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

	if (!mBasePassColor)
	{
		FRHITextureDesc Desc = FRHITextureDesc::Make2D(mSceneProxy->GetFrameWidth(), mSceneProxy->GetFrameHeight(), PF_R8G8B8A8).SetClearColor(Math::FLinearColor(0, 0, 0, 1)).SetFlags(ETextureCreateFlags::RenderTargetable);
		mBasePassColor = GDynamicRHI->RHICreateTexture(Desc);

	}
	if (!mBasePassDepth)
	{
		FRHITextureDesc Desc = FRHITextureDesc::Make2D(mSceneProxy->GetFrameWidth(), mSceneProxy->GetFrameHeight(), PF_DepthStencil).SetClearColor(FRHITextureClearColor(1.0, 0)).SetFlags(ETextureCreateFlags::DepthStencilTargetable);
		mBasePassDepth = GDynamicRHI->RHICreateTexture(Desc);
	}

	if (!mBasePass)
	{

		FRHIRenderPassDesc BasePassDesc;
		BasePassDesc.ColorRenderTargets[0] = { mBasePassColor.get(), nullptr, -1, 0, ALO_Clear, ASO_Store};
		BasePassDesc.NumColorRenderTargets = 1;
		BasePassDesc.DepthStencilRenderTarget  = { mBasePassDepth.get(), nullptr, ALO_Clear, ASO_Store, ALO_DontCare, ASO_DontCare};
		mBasePass = GDynamicRHI->RHICreateRenderPass(BasePassDesc);
	}

	if (FrameWidth != mSceneProxy->GetFrameWidth() || FrameHeight != mSceneProxy->GetFrameHeight())
	{
		FrameWidth = mSceneProxy->GetFrameWidth();
		FrameHeight = mSceneProxy->GetFrameHeight();
		GDynamicRHI->RHIResizeTexture(mBasePassColor.get(), FrameWidth, FrameHeight, 1);
		GDynamicRHI->RHIResizeTexture(mBasePassDepth.get(), FrameWidth, FrameHeight, 1);
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

	RHICmdList.GetContext()->BeginRenderPass(mBasePass.get());

	FRHIGraphicsPipelineStateDesc Desc;
	Desc.VertexInputState = GStaticMeshVertexInputState.VertexInputStateRHI.get();
	Desc.DepthStencilStateDesc.bEnableDepthWrite = true;
	Desc.DepthStencilStateDesc.DepthTestFunc = CF_Less;
	Desc.ColorBlendStateDesc.Attachments[0].bEnableBlend = true;
	RHICmdList.GetContext()->SetGraphicsPipelineState(Desc);

	mSceneProxy->SyncResource(RHICmdList);
	mSceneProxy->DrawScene(RHICmdList);

	RHICmdList.GetContext()->EndRenderPass(true);

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