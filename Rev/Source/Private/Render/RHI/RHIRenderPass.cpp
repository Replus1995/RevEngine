#include "Rev/Render/RHI/RHIRenderPass.h"

namespace Rev
{

bool FRHIRenderPass::SetRenderTarget(const Ref<FRHIRenderTarget>& InRenderTarget)
{
	const FRenderTargetDesc& RenderTargetDesc = InRenderTarget->GetDesc();
	const FRenderPassDesc& RenderPassDesc = GetDesc();

	if (RenderPassDesc.NumColorAttachments > RenderTargetDesc.NumColorTargets)
		return false;

	for (size_t i = 0; i < RenderPassDesc.NumColorAttachments; i++)
	{
		if (RenderPassDesc.ColorAttachments[i].Format != RenderTargetDesc.ColorTargets[i].Format)
			return false;
	}

	if(RenderPassDesc.DepthStencilAttchment.Format != RenderTargetDesc.DepthStencilTarget.Format)
		return false;

	mRenderTarget = InRenderTarget;

	return true;

}

}


