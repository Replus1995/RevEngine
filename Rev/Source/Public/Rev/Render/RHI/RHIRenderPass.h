#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RHI/RHIDefinitions.h"

namespace Rev
{
class FRHITexture;
struct FRHIRenderPassDesc
{
	struct FColorEntry
	{
		FRHITexture* ColorTarget = nullptr;
		FRHITexture* ResolveTarget = nullptr;
		int32 ArraySlice = -1;
		uint8 MipIndex = 0;
		ERenderTargetLoadAction LoadOp = RTL_DontCare;
		ERenderTargetStoreAction StoreOp = RTS_DontCare;
	};
	FColorEntry ColorRenderTargets[RTA_MaxColorAttachments];
	uint32 NumColorRenderTargets = 0;

	struct FDepthStencilEntry
	{
		FRHITexture* DepthStencilTarget = nullptr;
		FRHITexture* ResolveTarget = nullptr;
		ERenderTargetLoadAction DepthLoadOp = RTL_DontCare;
		ERenderTargetStoreAction DepthStoreOp = RTS_DontCare;
		ERenderTargetLoadAction StencilLoadOp = RTL_DontCare;
		ERenderTargetStoreAction StencilStoreOp = RTS_DontCare;
	};
	FDepthStencilEntry DepthStencilRenderTarget;
	//SubpassHint
};

class FRHIRenderPass : public FRHIResource
{
public:
	FRHIRenderPass(const FRHIRenderPassDesc& InDesc) : PassDesc(InDesc) {}
	virtual ~FRHIRenderPass() = default;
	virtual void MarkFramebufferDirty() = 0;
	const FRHIRenderPassDesc& GetDesc() const { return PassDesc; }

protected:
	FRHIRenderPassDesc PassDesc;

};


}