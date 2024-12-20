#pragma once
#include "Rev/Render/RHI/RHIDefinitions.h"
#include "Rev/Render/RHI/RHIResource.h"
#include "Rev/Math/Maths.h"
#include "Rev/Core/Hash.h"

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
	virtual bool GetDesc(struct FRHIRasterizerStateDesc& OutDesc) const { return false; }
};

class FRHIDepthStencilState
{
public:
	FRHIDepthStencilState() {}
	virtual bool GetDesc(struct FRHIDepthStencilStateDesc& OutDesc) const { return false; }
};

class FRHIColorBlendState
{
public:
	FRHIColorBlendState() {}
	virtual bool GetDesc(struct FRHIColorBlendStateDesc& OutDesc) const { return false; }
};

class FRHIVertexInputState
{
public:
	FRHIVertexInputState() {}
	virtual bool GetDesc(struct FRHIVertexInputStateDesc& OutDesc) const { return false; }
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

struct FRHISamplerStateDesc
{
	ESamplerFilterMode Filter = SF_Nearest;
	ESamplerWarpMode WarpU = SW_Repeat;
	ESamplerWarpMode WarpV = SW_Repeat;
	ESamplerWarpMode WarpW = SW_Repeat;
	float MipBias = 0.0f;
	int32 MaxAnisotropy = 0;
	float MinMipLevel = 0.0f;
	float MaxMipLevel = FLT_MAX;
	uint32 BorderColor = 0;
	ESamplerCompareFunction CompareFunc = SCF_Never;

	FRHISamplerStateDesc() {}
	FRHISamplerStateDesc(
		ESamplerFilterMode InFilter,
		ESamplerWarpMode InWarpU = SW_Repeat,
		ESamplerWarpMode InWarpV = SW_Repeat,
		ESamplerWarpMode InWarpW = SW_Repeat,
		float InMipBias = 0.0f,
		int32 InMaxAnisotropy = 0,
		float InMinMipLevel = 0,
		float InMaxMipLevel = FLT_MAX,
		uint32 InBorderColor = 0,
		/** Only supported in D3D11 */
		ESamplerCompareFunction InCompareFunc = SCF_Never
	)
		: Filter(InFilter)
		, WarpU(InWarpU)
		, WarpV(InWarpV)
		, WarpW(InWarpW)
		, MipBias(InMipBias)
		, MaxAnisotropy(InMaxAnisotropy)
		, MinMipLevel(InMinMipLevel)
		, MaxMipLevel(InMaxMipLevel)
		, BorderColor(InBorderColor)
		, CompareFunc(InCompareFunc)
	{
	}

	REV_API bool operator==(const FRHISamplerStateDesc& B) const;
};

struct FRHIRasterizerStateDesc
{
	EFillMode FillMode = PM_Fill;
	ECullMode CullMode = CM_None;

	float DepthBias = 0.0f;
	float DepthBiasSlopeFactor = 0.0f;
	EDepthClipMode DepthClipMode = DCM_Clip;
	bool bAllowMSAA = false;
	bool bEnableLineAA = false;

	REV_API bool operator==(const FRHIRasterizerStateDesc& B) const;
};

struct FRHIDepthStencilStateDesc
{
	bool bEnableDepthWrite = false;
	ECompareFunction DepthTestFunc = CF_Never;

	bool bEnableFrontFaceStencil = false;
	bool bEnableBackFaceStencil = false;

	EStencilOperation FrontFaceStencilFailOp = SO_Keep;
	EStencilOperation FrontFaceStencilPassOp = SO_Keep;
	EStencilOperation FrontFaceStencilDepthFailOp = SO_Keep;
	ECompareFunction FrontFaceStencilFunc = CF_Never;

	EStencilOperation BackFaceStencilFailOp = SO_Keep;
	EStencilOperation BackFaceStencilPassOp = SO_Keep;
	EStencilOperation BackFaceStencilDepthFailOp = SO_Keep;
	ECompareFunction BackFaceStencilFunc = CF_Never;

	uint8 StencilReadMask = 0;
	uint8 StencilWriteMask = 0;

	REV_API bool operator==(const FRHIDepthStencilStateDesc& B) const;

};

struct FRHIColorBlendStateDesc
{
	struct FAttachment {
		bool bEnableBlend = false;
		EBlendFactor SrcColorFactor = BF_One;
		EBlendFactor DstColorFactor = BF_Zero;
		EBlendOperation ColorOp = BO_Add;
		EBlendFactor SrcAlphaFactor = BF_One;
		EBlendFactor DstAlphaFactor = BF_Zero;
		EBlendOperation AlphaOp = BO_Add;
		EColorWriteMask ColorWriteMask = CWM_RGBA;
	};

	FAttachment Attachments[RTA_MaxColorAttachments];
	bool bUseAlphaToCoverage = false;
	//Math::FLinearColor BlendConstants;

	REV_API bool operator==(const FRHIColorBlendStateDesc& B) const;
};

struct FRHIVertexElement
{
	EVertexElmentType Type = EVertexElmentType::Unknown;
	uint8 StreamIndex = 0;
	uint8 AttributeIndex = 0;
	uint8 Offset = 0;
	uint16 Stride = 0;

	FRHIVertexElement() {}
	FRHIVertexElement(EVertexElmentType InType, uint8 InStreamIndex, uint8 InAttributeIndex, uint16 InStride, uint8 InOffset = 0)
		: Type(InType)
		, StreamIndex(InStreamIndex)
		, AttributeIndex(InAttributeIndex)
		, Stride(InStride)
		, Offset(InOffset)
	{}


	uint8 GetComponentSize();
	uint8 GetComponentCount();
	uint32 GetElementSize(); // GetComponentSize * ComponentCount

	REV_API bool operator==(const Rev::FRHIVertexElement& B) const;
};

struct FRHIVertexInputStateDesc
{
	FRHIVertexElement VertexElements[REV_MAX_VERTEX_ELEMENTS];
	uint8 NumVertexElements = 0;

	void Add(const FRHIVertexElement& InElement)
	{
		VertexElements[NumVertexElements] = InElement;
		NumVertexElements++;
	}

	REV_API bool operator==(const Rev::FRHIVertexInputStateDesc& B) const;
};


}

STD_MAP_HASH(Rev::FRHISamplerStateDesc)
STD_MAP_HASH(Rev::FRHIRasterizerStateDesc)
STD_MAP_HASH(Rev::FRHIDepthStencilStateDesc)
STD_MAP_HASH(Rev::FRHIColorBlendStateDesc)
STD_MAP_HASH(Rev::FRHIVertexInputStateDesc)