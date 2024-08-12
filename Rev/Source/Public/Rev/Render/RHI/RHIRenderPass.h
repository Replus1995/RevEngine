#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RHI/RHIRenderTarget.h"
#include "Rev/Render/RHI/RHIPipeline.h"

namespace Rev
{

struct FSubPassAttachmentRef
{
	ERenderTargetAttachment Attachment = RTA_EmptyAttachment;
	//Layout
};


struct FSubPassDesc
{
public:
	EPipelineBindPoint PipelineBindPoint;
	uint32 InputAttachmentCount;
	FSubPassAttachmentRef InputAttachments[RTA_MaxColorAttachments + 1];
	uint32 ColorAttachmentCount;
	FSubPassAttachmentRef ColorAttachments[RTA_MaxColorAttachments];
	//FSubPassAttachmentRef ResolveAttachments[RTA_MaxColorAttachments]; //for msaa
	FSubPassAttachmentRef DepthStencilAttachment;
	//preserved ?
};

class FRHISubPass
{
public:
	FRHISubPass(const FSubPassDesc& InDesc) : mDesc(InDesc) {}
	virtual ~FRHISubPass() = default;
	const FSubPassDesc& GetDesc() const { return mDesc; }

protected:
	FSubPassDesc mDesc;
	class FRHIRenderPass* mParentPass;
	Ref<FRHIPipeline> mPipeline;
};

struct FSubPassDependencyDesc
{
public:
	uint32 SrcIndex;
	uint32 DstIndex;
	//SrcStageMask
	//DstStageMask
	//SrcAccessMask
	//DstAccessMask
};

class FRHISubPassDependency
{
public:
	FRHISubPassDependency(const FSubPassDependencyDesc& InDesc) : mDesc(InDesc) {}
	virtual ~FRHISubPassDependency() = default;

protected:
	FSubPassDependencyDesc mDesc;
};

enum EAttachmentLoadOp : uint8
{
	ALO_Load = 0,
	ALO_Clear,
	ALO_DontCare,
	ALO_NoneExt
};

enum EAttachmentStoreOp : uint8
{
	ASO_Store = 0,
	ASO_DontCare,
	ASO_NoneExt
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
};

class FRHIRenderPass
{
public:
	FRHIRenderPass();
	virtual ~FRHIRenderPass();

};


}