#include "Rev/Render/RHI/RHIRenderTarget.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

FRHIRenderTarget::FRHIRenderTarget(const FRenderTargetSource* InColorSources, uint32 InNumColorSources, const FRenderTargetSource& InDepthSource)
{
	RE_CORE_ASSERT(InNumColorSources > 0 || InDepthSource.Texture != nullptr);
	for (uint32 i = 0; i < InNumColorSources; i++)
	{
		const FRenderTargetSource& ColorSource = InColorSources[i];
		if (!ColorSource.Texture)
			continue;
		if (mDesc.Width == 0)
			mDesc.Width = ColorSource.Texture->GetWidth();
		if (mDesc.Height == 0)
			mDesc.Height = ColorSource.Texture->GetHeight();
		FColorTargetDesc& ColorDesc = mDesc.ColorTargets[i];
		ColorDesc.Format = ColorSource.Texture->GetFormat();
		ColorDesc.ClearColor = ColorSource.Texture->GetDesc().ClearColor.RGBA;
		mDesc.NumColorTargets++;
	}
	if (InDepthSource.Texture != nullptr)
	{
		if (mDesc.Width == 0)
			mDesc.Width = InDepthSource.Texture->GetWidth();
		if (mDesc.Height == 0)
			mDesc.Height = InDepthSource.Texture->GetHeight();

		mDesc.DepthStencilTarget.Format = InDepthSource.Texture->GetFormat();
		mDesc.DepthStencilTarget.ClearDepth = InDepthSource.Texture->GetDesc().ClearColor.Depth;
		mDesc.DepthStencilTarget.ClearStencil = InDepthSource.Texture->GetDesc().ClearColor.Stencil;
	}
}

}