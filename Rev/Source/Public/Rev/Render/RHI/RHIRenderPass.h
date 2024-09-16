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
	ERenderTargetAttachment DepthStencilAttachment;
	ERenderTargetAttachment InputAttachments[RTA_MaxColorAttachments + 2];
	uint32 ColorAttachmentCount = 0;
	uint32 InputAttachmentCount = 0;
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

struct FRenderPassAttachmentDesc
{
public:
	EPixelFormat Format;
	EAttachmentLoadOp LoadOp;
	EAttachmentStoreOp StoreOp;
	EAttachmentLoadOp StencilLoadOp;
	EAttachmentStoreOp StencilStoreOp;
};


struct FRenderPassDesc
{
public:
	uint32 AttachmentCount = 0;
	FRenderPassAttachmentDesc Attachments[RTA_MaxColorAttachments + 1];
	std::vector<FSubpassDesc> SubpassDescs;
	std::vector<FSubpassDependDesc> SubpassDependDescs;
};

class FRHIRenderPass : public FRHIResource
{
public:
	FRHIRenderPass(const FRenderPassDesc& InDesc) : mDesc(InDesc) {}
	virtual ~FRHIRenderPass() = default;

protected:
	FRenderPassDesc mDesc;
	Ref<FRHIRenderTarget> mRenderTarget;

};


}