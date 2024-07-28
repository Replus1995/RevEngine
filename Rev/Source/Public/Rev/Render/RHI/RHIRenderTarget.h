#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/Assert.h"
#include "Rev/Render/PixelFormat.h"
#include "Rev/Render/RHI/RHIResource.h"
#include "Rev/Render/RHI/RHITexture.h"
#include "Rev/Math/Maths.h"

namespace Rev
{

enum ERenderTargetAttachment : uint8
{
	RTA_ColorAttachment0 = 0,
	RTA_ColorAttachment1 = 1,
	RTA_ColorAttachment2 = 2,
	RTA_ColorAttachment3 = 3,
	RTA_ColorAttachment4 = 4,
	RTA_ColorAttachment5 = 5,
	RTA_ColorAttachment6 = 6,
	RTA_ColorAttachment7 = 7,
	RTA_MaxColorAttachments = 8,
	RTA_DepthStencilAttachment = 9,
};

struct FColorTargetDesc
{
	EPixelFormat Format = PF_Unknown;
	Math::FLinearColor ClearColor = Math::FLinearColor(0,0,0,1);
	//TODO: Load/Store actions
};

struct FDepthStencilTargetDesc
{
	EPixelFormat Format = PF_Unknown;
	float ClearDepth = 1.0F;
	uint32 ClearStencil = 0;
	//TODO: Load/Store actions
};

struct FRenderTargetDesc
{
	uint32 Width = 0;
	uint32 Height = 0;
	FColorTargetDesc ColorTargets[RTA_MaxColorAttachments] = {};
	uint8 NumColorTargets = 0;
	FDepthStencilTargetDesc DepthStencilTarget;
	uint8 NumSamples = 1; //MSAA

	FRenderTargetDesc() = default;
	FRenderTargetDesc(
		uint32 InWidth, uint32 InHeight,
		const FColorTargetDesc* InColorTargets,
		uint32 InNumColorTargets,
		const FDepthStencilTargetDesc& InDepthStencilTarget,
		uint8 InNumSamples = 1
	)
		: Width(InWidth)
		, Height(InHeight)
		, DepthStencilTarget(InDepthStencilTarget)
		, NumSamples(InNumSamples)
	{
		RE_CORE_ASSERT(InNumColorTargets <= RTA_MaxColorAttachments);
		NumColorTargets = InNumColorTargets;
		for (size_t i = 0; i < InNumColorTargets; i++)
		{
			ColorTargets[i] = InColorTargets[i];
		}
	}
};

struct FRenderTargetSource
{
	ERenderTargetAttachment Attachment;
	uint8 MipLevel;
	uint16 ArrayIndex;
	Ref<FRHITexture> Texture;

	FRenderTargetSource(
		ERenderTargetAttachment InAttachment, const Ref<FRHITexture>& InTexture, 
		uint16 InArrayIndex = 0, uint8 InMipLevel = 0
	)
		: Attachment(InAttachment)
		, MipLevel(InMipLevel)
		, ArrayIndex(InArrayIndex)
		, Texture(InTexture)
	{}

};

class FRHIRenderTarget : public FRHIResource
{
public:
	virtual ~FRHIRenderTarget() = default;

	virtual void Bind() = 0;
	virtual void Unbind() = 0;

	virtual const FRenderTargetDesc& GetDesc() const { return mDesc; };

	virtual void ClearTarget(ERenderTargetAttachment Index) = 0;
	virtual const Ref<FRHITexture> GetTargetTexture(ERenderTargetAttachment Index) const = 0;
	virtual void Recreate(uint32 InWidth, uint32 InHeight) = 0;

protected:
	FRHIRenderTarget(const FRenderTargetDesc& InDesc) : mDesc(InDesc) {}
	FRHIRenderTarget(const FRenderTargetSource* InSources, uint32 InNumSources);
protected:
	FRenderTargetDesc mDesc;
};


}