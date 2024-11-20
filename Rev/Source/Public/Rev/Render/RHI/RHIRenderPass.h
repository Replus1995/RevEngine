#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RHI/RHIDefinitions.h"
#include "Rev/Render/RHI/RHIResource.h"

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
		ERenderTargetLoadAction LoadAction = RTL_DontCare;
		ERenderTargetStoreAction StoreAction = RTS_DontCare;
	};
	FColorEntry ColorRenderTargets[RTA_MaxColorAttachments];
	uint32 NumColorRenderTargets = 0;

	struct FDepthStencilEntry
	{
		FRHITexture* DepthStencilTarget = nullptr;
		FRHITexture* ResolveTarget = nullptr;
		ERenderTargetLoadAction DepthLoadAction = RTL_DontCare;
		ERenderTargetStoreAction DepthStoreAction = RTS_DontCare;
		ERenderTargetLoadAction StencilLoadAction = RTL_DontCare;
		ERenderTargetStoreAction StencilStoreAction = RTS_DontCare;
	};
	FDepthStencilEntry DepthStencilRenderTarget;
	//SubpassHint
};

class FRHIRenderPass : public FRHIResource
{
public:
	FRHIRenderPass(const FRHIRenderPassDesc& InDesc) : PassDesc(InDesc) {}
	virtual ~FRHIRenderPass() = default;

	const FRHIRenderPassDesc& GetDesc() const { return PassDesc; }

protected:
	FRHIRenderPassDesc PassDesc;

};


}