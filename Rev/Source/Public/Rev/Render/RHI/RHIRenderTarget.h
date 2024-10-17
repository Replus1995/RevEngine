#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/Assert.h"
#include "Rev/Render/PixelFormat.h"
#include "Rev/Render/RHI/RHIDefinitions.h"
#include "Rev/Render/RHI/RHIResource.h"
#include "Rev/Render/RHI/RHITexture.h"
#include "Rev/Math/Maths.h"

namespace Rev
{

enum class ERenderTargetDimension : uint8
{
	RenderTargetEmpty = 0,
	RenderTarget2D = 1,
	RenderTarget2DArray = 2,
	RenderTargetCube = 3,
	RenderTargetCubeArray = 4
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
	uint16 Width = 0;
	uint16 Height = 0;
	ERenderTargetDimension Dimension = ERenderTargetDimension::RenderTargetEmpty;
	uint16 ArraySize = 1;
	FColorTargetDesc ColorTargets[RTA_MaxColorAttachments] = {};
	uint8 NumColorTargets = 0;
	FDepthStencilTargetDesc DepthStencilTarget;
	uint8 NumSamples = 1; //MSAA

	FRenderTargetDesc() = default;
	FRenderTargetDesc(
		ERenderTargetDimension InDimension,
		uint16 InWidth, uint16 InHeight,
		const FColorTargetDesc* InColorTargets,
		uint32 InNumColorTargets,
		const FDepthStencilTargetDesc& InDepthStencilTarget,
		uint16 InArraySize = 1, uint8 InNumSamples = 1
	)
		: Width(InWidth)
		, Height(InHeight)
		, Dimension(InDimension)
		, ArraySize(InArraySize)
		, DepthStencilTarget(InDepthStencilTarget)
		, NumSamples(InNumSamples)
	{
		REV_CORE_ASSERT(InNumColorTargets <= RTA_MaxColorAttachments);
		NumColorTargets = InNumColorTargets;
		for (size_t i = 0; i < InNumColorTargets; i++)
		{
			ColorTargets[i] = InColorTargets[i];
		}
	}

	static FRenderTargetDesc MakeEmpty()
	{
		return FRenderTargetDesc();
	}

	static FRenderTargetDesc Make2D(uint16 InWidth, uint16 InHeight, const FColorTargetDesc* InColorTargets, uint32 InNumColorTargets, const FDepthStencilTargetDesc& InDepthStencilTarget, uint8 InNumSamples = 1)
	{
		return FRenderTargetDesc(ERenderTargetDimension::RenderTarget2D, InWidth, InHeight, InColorTargets, InNumColorTargets, InDepthStencilTarget, 1, InNumSamples);
	}

	static FRenderTargetDesc Make2DArray(uint16 InWidth, uint16 InHeight, const FColorTargetDesc* InColorTargets, uint32 InNumColorTargets, const FDepthStencilTargetDesc& InDepthStencilTarget, uint16 InArraySize, uint8 InNumSamples = 1)
	{
		return FRenderTargetDesc(ERenderTargetDimension::RenderTarget2DArray, InWidth, InHeight, InColorTargets, InNumColorTargets, InDepthStencilTarget, InArraySize, InNumSamples);
	}

	static FRenderTargetDesc MakeCube(uint16 InWidth, uint16 InHeight, const FColorTargetDesc* InColorTargets, uint32 InNumColorTargets, const FDepthStencilTargetDesc& InDepthStencilTarget, uint8 InNumSamples = 1)
	{
		return FRenderTargetDesc(ERenderTargetDimension::RenderTargetCube, InWidth, InHeight, InColorTargets, InNumColorTargets, InDepthStencilTarget, 1, InNumSamples);
	}

	static FRenderTargetDesc MakeCubeArray(uint16 InWidth, uint16 InHeight, const FColorTargetDesc* InColorTargets, uint32 InNumColorTargets, const FDepthStencilTargetDesc& InDepthStencilTarget, uint16 InArraySize, uint8 InNumSamples = 1)
	{
		return FRenderTargetDesc(ERenderTargetDimension::RenderTargetCube, InWidth, InHeight, InColorTargets, InNumColorTargets, InDepthStencilTarget, InArraySize, InNumSamples);
	}
};

class FRHIRenderTarget : public FRHIResource
{
public:
	virtual ~FRHIRenderTarget() = default;
	const FRenderTargetDesc& GetDesc() const { return mDesc; };

	virtual void ResizeTargets(uint16 InWidth, uint16 InHeight) = 0;
	virtual void ClearTarget(ERenderTargetAttachment Index) = 0;
	virtual void ClearTargets() = 0;
	virtual const FRHITexture* GetTargetTexture(ERenderTargetAttachment Index) const = 0;

	uint16 GetWidth() const { return mDesc.Width; }
	uint16 GetHeight() const { return mDesc.Height; }

	//Use textures created by user
	/*virtual void Attach(ERenderTargetAttachment Index, const Ref<FRHITexture>& InTexture, uint8 InMipLevel = 0, int32 InArrayIndex = -1) = 0;
	virtual void Detach(ERenderTargetAttachment Index) = 0;
	virtual void DetachAll() = 0;
	virtual void FlushAttach() = 0;*/

protected:
	FRHIRenderTarget(const FRenderTargetDesc& InDesc) : mDesc(InDesc) {}
protected:
	FRenderTargetDesc mDesc;
};


}