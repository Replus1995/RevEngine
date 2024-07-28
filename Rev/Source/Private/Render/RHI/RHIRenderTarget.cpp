#include "Rev/Render/RHI/RHIRenderTarget.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FRHIRenderTarget::FRHIRenderTarget(const FRenderTargetSource* InSources, uint32 InNumSources)
{
	RE_CORE_ASSERT(InNumSources > 0);
	for (uint32 i = 0; i < InNumSources; i++)
	{
		const FRenderTargetSource& Source = InSources[i];
		if (!Source.Texture)
			continue;
		if (mDesc.Width == 0)
			mDesc.Width = Source.Texture->GetWidth();
		if (mDesc.Height == 0)
			mDesc.Height = Source.Texture->GetHeight();
		if (Source.Attachment < RTA_MaxColorAttachments)
		{
			FColorTargetDesc& ColorDesc = mDesc.ColorTargets[Source.Attachment];
			ColorDesc.Format = Source.Texture->GetFormat();
			ColorDesc.ClearColor = Source.Texture->GetDesc().ClearColor.RGBA;
			mDesc.NumColorTargets++;
		}
		else if (Source.Attachment == RTA_DepthStencilAttachment)
		{
			mDesc.DepthStencilTarget.Format = Source.Texture->GetFormat();
			mDesc.DepthStencilTarget.ClearDepth = Source.Texture->GetDesc().ClearColor.Depth;
			mDesc.DepthStencilTarget.ClearStencil = Source.Texture->GetDesc().ClearColor.Stencil;
		}
		else
			RE_CORE_ASSERT(false, "Unsupported attachement");
	}
}

}