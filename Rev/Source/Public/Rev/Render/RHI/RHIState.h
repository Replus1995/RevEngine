#pragma once
#include "Rev/Render/RHI/RHIDefinitions.h"
#include "Rev/Render/RHI/RHIResource.h"
#include "Rev/Math/Maths.h"

namespace Rev
{

//TODO: Add FSamplerDesc hash function

//State
class FRHISamplerState
{
public:
	FRHISamplerState() {}
	//virtual bool GetDesc(struct FSamplerStateDesc& OutDesc) const { return false; };
};

class FRHIRasterizerState
{
public:
	FRHIRasterizerState() {}
	virtual bool GetDesc(struct FRasterizerStateDesc& OutDesc) const { return false; }
};

class FRHIDepthStencilState
{
public:
	FRHIDepthStencilState() {}
	virtual bool GetDesc(struct FDepthStencilStateDesc& OutDesc) const { return false; }
};

class FRHIColorBlendState
{
public:
	FRHIColorBlendState() {}
	virtual bool GetDesc(class FColorBlendStateDesc& OutDesc) const { return false; }
};

template <typename TRHIState, typename TRHIStateDesc>
static bool MatchRHIState(TRHIState* LHSState, TRHIState* RHSState)
{
	TRHIStateDesc LHSStateDesc;
	TRHIStateDesc RHSStateDesc;
	if (LHSState)
	{
		LHSState->GetDesc(LHSStateDesc);
	}
	if (RHSState)
	{
		RHSState->GetDesc(RHSStateDesc);
	}
	return LHSStateDesc == RHSStateDesc;
}


//State Desc

struct FSamplerStateDesc
{
	ESamplerFilterMode Filter = SF_Nearest;
	ESamplerWarpMode WarpU = SW_Repeat;
	ESamplerWarpMode WarpV = SW_Repeat;
	ESamplerWarpMode WarpW = SW_Repeat;
	float MipBias = 0.0f;
	float MinMipLevel = 0.0f;
	float MaxMipLevel = FLT_MAX;
	int32 MaxAnisotropy = 0;
	uint32 BorderColor = 0;
	ESamplerCompareFunction CompareFunction = SCF_Never;

	REV_API friend bool operator==(const FSamplerStateDesc& A, const FSamplerStateDesc& B);
};

struct FRasterizerStateDesc
{
	EFillMode FillMode = PM_Fill;
	ECullMode CullMode = CM_None;

	float DepthBias = 0.0f;
	float DepthBiasSlopeFactor = 0.0f;
	EDepthClipMode DepthClipMode = DCM_Clip;
	bool bAllowMSAA = false;
	bool bEnableLineAA = false;
};

struct FDepthStencilStateDesc
{
	bool bEnableDepthWrite = false;
	ECompareFunction DepthTestFunc = CF_Never;

	bool bEnableFrontFaceStencil;
	bool bEnableBackFaceStencil;

	EStencilOperation FrontFaceStencilFailOp;
	EStencilOperation FrontFaceStencilPassOp;
	EStencilOperation FrontFaceStencilDepthFailOp;
	ECompareFunction FrontFaceStencilFunc;

	EStencilOperation BackFaceStencilFailOp;
	EStencilOperation BackFaceStencilPassOp;
	EStencilOperation BackFaceStencilDepthFailOp;
	ECompareFunction BackFaceStencilFunc;

	uint8 StencilReadMask;
	uint8 StencilWriteMask;

};

struct FColorBlendStateDesc
{
	struct FColorTarget {
		bool bEnableBlend = false;
		EBlendFactor SrcColorFactor = BF_One;
		EBlendFactor DstColorFactor = BF_Zero;
		EBlendOperation ColorOp = BO_Add;
		EBlendFactor SrcAlphaFactor = BF_One;
		EBlendFactor DstAlphaFactor = BF_Zero;
		EBlendOperation AlphaOp = BO_Add;
		EColorWriteMask ColorWriteMask = CWM_RGBA;
	};

	FColorTarget ColorTargets[RTA_MaxColorAttachments];
	Math::FLinearColor BlendConstants;
};




}