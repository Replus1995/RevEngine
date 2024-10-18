#include "Rev/Render/Renderer/ForwardRenderer.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHIContext.h"
#include "Rev/Render/RHI/RHICommandList.h"
#include "Rev/Render/RHI/RHIPipeline.h"
#include "Rev/Render/RenderProxy/SceneProxy.h"

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

	if (!mBasePass)
	{
		FSubpassDesc SubpassDesc;
		SubpassDesc.PipelineBindPoint = PBP_Graphics;
		SubpassDesc.ColorAttachments[0] = RTA_ColorAttachment0;
		SubpassDesc.DepthStencilAttachment = RTA_DepthStencilAttachment;
		SubpassDesc.NumColorAttachments = 1;
		SubpassDesc.NumInputAttachments = 0;


		FRenderPassDesc BasePassDesc;
		BasePassDesc.ColorAttachments[0] = {PF_R8G8B8A8, ALO_Clear, ASO_Store};
		BasePassDesc.NumColorAttachments = 1;
		BasePassDesc.DepthStencilAttchment  = {PF_DepthStencil, ALO_Clear, ASO_Store, ALO_DontCare, ASO_DontCare};
		BasePassDesc.SubpassDescs.push_back(SubpassDesc);

		mBasePass = GDynamicRHI->CreateRenderPass(BasePassDesc);
	}

	if (!mBasePassTarget)
	{
		std::vector<FColorTargetDesc> vColorDesc;
		vColorDesc.push_back({ PF_R8G8B8A8, Math::FLinearColor(0, 0, 0, 1) });
		FRenderTargetDesc Desc = FRenderTargetDesc::Make2D(mSceneProxy->GetFrameWidth(), mSceneProxy->GetFrameHeight(), vColorDesc.data(), vColorDesc.size(), {PF_DepthStencil});
		mBasePassTarget = GDynamicRHI->CreateRenderTarget(Desc);


		mBasePass->SetRenderTarget(mBasePassTarget);
		//RenderCmd::BindTexture(mLinearScreenTarget->GetTargetTexture(RTA_ColorAttachment0), UL::SLinearScreenTex); //to be optimized
	}

	mBasePassTarget->ResizeTargets(mSceneProxy->GetFrameWidth(), mSceneProxy->GetFrameHeight());

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

	RHICmdList.GetContext()->BeginRenderPass(mBasePass);

	FRHIGraphicsPipelineStateDesc Desc;
	Desc.DepthStencilStateDesc.bEnableDepthWrite = true;
	Desc.DepthStencilStateDesc.DepthTestFunc = CF_Less;
	Desc.ColorBlendStateDesc.ColorTargets[0].bEnableBlend = true;
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