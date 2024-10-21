#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RHI/RHIDefinitions.h"
#include "Rev/Render/RHI/RHIRenderTarget.h"

namespace Rev
{

struct FRHISubpassDesc
{
public:
	EPipelineBindPoint PipelineBindPoint = PBP_Graphics;
	ERenderTargetAttachment ColorAttachments[RTA_MaxColorAttachments];
	ERenderTargetAttachment ResolveAttachments[RTA_MaxColorAttachments];
	ERenderTargetAttachment DepthStencilAttachment = RTA_EmptyAttachment;
	ERenderTargetAttachment InputAttachments[RTA_MaxColorAttachments + 2];
	uint32 NumColorAttachments = 0;
	uint32 NumInputAttachments = 0;
};

struct FRHISubpassDependDesc
{
public:
	uint32 SrcIndex;
	uint32 DstIndex;
	//SrcStageMask
	//DstStageMask
	//SrcAccessMask
	//DstAccessMask
};

struct FRHIColorAttachmentDesc
{
public:
	EPixelFormat Format;
	EAttachmentLoadOp LoadOp;
	EAttachmentStoreOp StoreOp;
};

struct FRHIDepthStencilAttachmentDesc : public FRHIColorAttachmentDesc
{
public:
	EAttachmentLoadOp StencilLoadOp;
	EAttachmentStoreOp StencilStoreOp;
};


struct FRHIRenderPassDesc
{
public:
	FRHIColorAttachmentDesc ColorAttachments[RTA_MaxColorAttachments];
	uint32 NumColorAttachments = 0;
	FRHIDepthStencilAttachmentDesc DepthStencilAttchment;
	std::vector<FRHISubpassDesc> SubpassDescs;  //TODO: define MAX_SUB_PASS to make this member as an array
	//std::vector<FSubpassDependDesc> SubpassDependDescs;
};

class FRHIRenderPass : public FRHIResource
{
public:
	FRHIRenderPass(const FRHIRenderPassDesc& InDesc) : mDesc(InDesc) {}
	virtual ~FRHIRenderPass() = default;

	const FRHIRenderPassDesc& GetDesc() const { return mDesc; }
	const Ref<FRHIRenderTarget>& GetRenderTarget() const { return mRenderTarget; } 
	bool SetRenderTarget(const Ref<FRHIRenderTarget>& InRenderTarget);

protected:
	FRHIRenderPassDesc mDesc;
	Ref<FRHIRenderTarget> mRenderTarget;

};


}