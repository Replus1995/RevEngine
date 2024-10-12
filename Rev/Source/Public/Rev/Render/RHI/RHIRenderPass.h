#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RHI/RHIRenderTarget.h"
#include "Rev/Render/RHI/RHIPipeline.h"

namespace Rev
{

struct FSubpassDesc
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

struct FSubpassDependDesc
{
public:
	uint32 SrcIndex;
	uint32 DstIndex;
	//SrcStageMask
	//DstStageMask
	//SrcAccessMask
	//DstAccessMask
};

enum EAttachmentLoadOp : uint8
{
	ALO_Load = 0,
	ALO_Clear,
	ALO_DontCare
};

enum EAttachmentStoreOp : uint8
{
	ASO_Store = 0,
	ASO_DontCare
};

struct FColorAttachmentDesc
{
public:
	EPixelFormat Format;
	EAttachmentLoadOp LoadOp;
	EAttachmentStoreOp StoreOp;
};

struct FDepthStencilAttachmentDesc : public FColorAttachmentDesc
{
public:
	EAttachmentLoadOp StencilLoadOp;
	EAttachmentStoreOp StencilStoreOp;
};



struct FRenderPassDesc
{
public:
	FColorAttachmentDesc ColorAttachments[RTA_MaxColorAttachments];
	uint32 NumColorAttachments = 0;
	FDepthStencilAttachmentDesc DepthStencilAttchment;
	std::vector<FSubpassDesc> SubpassDescs;  //TODO: define MAX_SUB_PASS to make this member as an array
	std::vector<FSubpassDependDesc> SubpassDependDescs;
};

class FRHIRenderPass : public FRHIResource
{
public:
	FRHIRenderPass(const FRenderPassDesc& InDesc) : mDesc(InDesc) {}
	virtual ~FRHIRenderPass() = default;

	const FRenderPassDesc& GetDesc() const { return mDesc; }
	const Ref<FRHIRenderTarget>& GetRenderTarget() const { return mRenderTarget; } 
	bool SetRenderTarget(const Ref<FRHIRenderTarget>& InRenderTarget);

protected:
	FRenderPassDesc mDesc;
	Ref<FRHIRenderTarget> mRenderTarget;

};


}