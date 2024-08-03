#include "Rev/Render/RenderPipeline/ForwardPipeline.h"
#include "Rev/Render/RHI/RHIResourceFactory.h"
#include "Rev/Render/UniformLayout.h"

namespace Rev
{

FForwardPipeline::FForwardPipeline()
{
}

FForwardPipeline::~FForwardPipeline()
{
}

void FForwardPipeline::BeginPipeline(uint32 InWidth, uint32 InHeight, SceneRenderProxy* InSceneProxy)
{
	FRenderPipeline::BeginPipeline(InWidth, InHeight, InSceneProxy);
	if (!mLinearScreenTarget)
	{
		std::vector<FColorTargetDesc> vColorDesc;
		vColorDesc.push_back({ PF_R8G8B8A8, Math::FLinearColor(0, 0, 0, 1) });
		FRenderTargetDesc Desc = FRenderTargetDesc::Make2D(InWidth, InHeight, vColorDesc.data(), vColorDesc.size(), {PF_DepthStencil});
		mLinearScreenTarget = FRHIResourceFactory::CreateRenderTarget(Desc);
		mLinearScreenTarget->GetTargetTexture(RTA_ColorAttachment0)->Bind(UL::SLinearScreenTex);
		mForwardSurfacePass.SetRenderTarget(mLinearScreenTarget);
	}
	else
	{
		mLinearScreenTarget->ResizeTargets(InWidth, InHeight);
	}
}

void FForwardPipeline::RunPipeline()
{
	FRenderPipeline::RunPipeline();

	RunPass(&mForwardSurfacePass);
	RunPass(&mGammaCorrectPass);

}

void FForwardPipeline::EndPipeline()
{
	FRenderPipeline::EndPipeline();
}

}