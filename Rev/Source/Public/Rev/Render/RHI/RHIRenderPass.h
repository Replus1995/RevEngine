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
		EAttachmentLoadOp LoadOp = ALO_DontCare;
		EAttachmentStoreOp StoreOp = ASO_DontCare;
	};
	FColorEntry ColorRenderTargets[RTA_MaxColorAttachments];
	uint32 NumColorRenderTargets = 0;

	struct FDepthStencilEntry
	{
		FRHITexture* DepthStencilTarget = nullptr;
		FRHITexture* ResolveTarget = nullptr;
		EAttachmentLoadOp DepthLoadOp = ALO_DontCare;
		EAttachmentStoreOp DepthStoreOp = ASO_DontCare;
		EAttachmentLoadOp StencilLoadOp = ALO_DontCare;
		EAttachmentStoreOp StencilStoreOp = ASO_DontCare;
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