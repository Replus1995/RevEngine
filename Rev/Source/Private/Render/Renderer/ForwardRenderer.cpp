#include "Rev/Render/Renderer/ForwardRenderer.h"
#include "Rev/Render/RHI/RHICore.h"
#include "Rev/Render/UniformLayout.h"
#include "Rev/Render/RenderCmd.h"
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

		mBasePass = FRHICore::CreateRenderPass(BasePassDesc);
	}

	if (!mBasePassTarget)
	{
		std::vector<FColorTargetDesc> vColorDesc;
		vColorDesc.push_back({ PF_R8G8B8A8, Math::FLinearColor(0, 0, 0, 1) });
		FRenderTargetDesc Desc = FRenderTargetDesc::Make2D(mSceneProxy->GetFrameWidth(), mSceneProxy->GetFrameHeight(), vColorDesc.data(), vColorDesc.size(), {PF_DepthStencil});
		mBasePassTarget = FRHICore::CreateRenderTarget(Desc);


		mBasePass->SetRenderTarget(mBasePassTarget);
		//RenderCmd::BindTexture(mLinearScreenTarget->GetTargetTexture(RTA_ColorAttachment0), UL::SLinearScreenTex); //to be optimized
	}

	mBasePassTarget->ResizeTargets(mSceneProxy->GetFrameWidth(), mSceneProxy->GetFrameHeight());

	//else
	//{
	//	mLinearScreenTarget->ResizeTargets(mContext->Width, mContext->Height);
	//}
	
}

void FForwardRenderer::DrawFrame()
{
	// (vulkan)
	// begin cmd buffer 
		// begin render pass
			// begin sub pass
			// end sub pass
		// end render pass
	// end cmd buffer

	RenderCmd::BeginRenderPass(mBasePass);
	mSceneProxy->SyncResource();
	mSceneProxy->DrawScene();

	RenderCmd::EndRenderPass(true);

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