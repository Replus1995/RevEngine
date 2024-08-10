#include "Rev/Render/Renderer/ForwardRenderer.h"
#include "Rev/Render/RHI/RHIResourceFactory.h"
#include "Rev/Render/UniformLayout.h"
#include "Rev/Render/RenderCmd.h"
#include "Rev/Render/RenderProxy/SceneProxy.h"

namespace Rev
{

FForwardRenderer::FForwardRenderer(const Ref<FRenderContext>& InContext)
	: FRenderer(InContext)
{
}

FForwardRenderer::~FForwardRenderer()
{
}

void FForwardRenderer::BeginFrame()
{
	//prepare resource
	if (!mLinearScreenTarget)
	{
		std::vector<FColorTargetDesc> vColorDesc;
		vColorDesc.push_back({ PF_R8G8B8A8, Math::FLinearColor(0, 0, 0, 1) });
		FRenderTargetDesc Desc = FRenderTargetDesc::Make2D(mContext->Width, mContext->Height, vColorDesc.data(), vColorDesc.size(), { PF_DepthStencil });
		mLinearScreenTarget = FRHIResourceFactory::CreateRenderTarget(Desc);


		mForwardSurfacePass.SetRenderTarget(mLinearScreenTarget);
		RenderCmd::BindTexture(mLinearScreenTarget->GetTargetTexture(RTA_ColorAttachment0), UL::SLinearScreenTex); //to be optimized
	}
	else
	{
		mLinearScreenTarget->ResizeTargets(mContext->Width, mContext->Height);
	}
	
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

	mForwardSurfacePass.BeginPass();
	mForwardSurfacePass.ClearRenderTarget();
	mContext->SceneProxy->SyncResource();
	mContext->SceneProxy->DrawScene();
	mForwardSurfacePass.EndPass();


	RenderCmd::SetCullFaceMode(CFM_Back);

	mGammaCorrectPass.BeginPass();
	mGammaCorrectPass.RunPass();
	mGammaCorrectPass.EndPass();


}

void FForwardRenderer::EndFrame()
{
}

}