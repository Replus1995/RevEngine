#include "Rev/Render/RHI/RHIState.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

bool operator==(const FRHISamplerStateDesc& A, const FRHISamplerStateDesc& B)
{
	bool bSame = A.Filter == B.Filter &&
		A.WarpU == B.WarpU &&
		A.WarpV == B.WarpV &&
		A.WarpW == B.WarpW &&
		A.MipBias == B.MipBias &&
		A.MinMipLevel == B.MinMipLevel &&
		A.MaxMipLevel == B.MaxMipLevel &&
		A.MaxAnisotropy == B.MaxAnisotropy && 
		A.BorderColor == B.BorderColor &&
		A.CompareFunc == B.CompareFunc;
	return bSame;
}

bool operator==(const FRHIRasterizerStateDesc& A, const FRHIRasterizerStateDesc& B)
{
	bool bSame = A.FillMode == B.FillMode &&
		A.CullMode == B.CullMode &&
		A.DepthBias == B.DepthBias &&
		A.DepthBiasSlopeFactor == B.DepthBiasSlopeFactor &&
		A.DepthClipMode == B.DepthClipMode &&
		A.bAllowMSAA == B.bAllowMSAA;
	return bSame;
}

bool operator==(const FRHIDepthStencilStateDesc& A, const FRHIDepthStencilStateDesc& B)
{
	bool bSame = A.bEnableDepthWrite == B.bEnableDepthWrite &&
		A.DepthTestFunc == B.DepthTestFunc &&
		A.bEnableFrontFaceStencil == B.bEnableFrontFaceStencil &&
		A.bEnableBackFaceStencil == B.bEnableBackFaceStencil &&
		A.FrontFaceStencilFailOp == B.FrontFaceStencilFailOp &&
		A.FrontFaceStencilPassOp == B.FrontFaceStencilPassOp &&
		A.FrontFaceStencilDepthFailOp == B.FrontFaceStencilDepthFailOp &&
		A.FrontFaceStencilFunc == B.FrontFaceStencilFunc &&
		A.BackFaceStencilFailOp == B.BackFaceStencilFailOp &&
		A.BackFaceStencilPassOp == B.BackFaceStencilPassOp &&
		A.BackFaceStencilDepthFailOp == B.BackFaceStencilDepthFailOp &&
		A.BackFaceStencilFunc == B.BackFaceStencilFunc &&
		A.StencilReadMask == B.StencilReadMask &&
		A.StencilWriteMask == B.StencilWriteMask;
	return bSame;
}

bool operator==(const FRHIColorBlendStateDesc& A, const FRHIColorBlendStateDesc& B)
{
	bool bSame = true;
	for (uint8 i = 0; i < RTA_MaxColorAttachments; i++)
	{
		const FRHIColorBlendStateDesc::FAttachment& AA = A.Attachments[i];
		const FRHIColorBlendStateDesc::FAttachment& BB = B.Attachments[i];

		bSame &= AA.bEnableBlend == BB.bEnableBlend &&
			AA.SrcColorFactor == BB.SrcColorFactor &&
			AA.DstColorFactor == BB.DstColorFactor &&
			AA.ColorOp == BB.ColorOp &&
			AA.SrcAlphaFactor == BB.SrcAlphaFactor &&
			AA.DstAlphaFactor == BB.DstAlphaFactor &&
			AA.AlphaOp == BB.AlphaOp &&
			AA.ColorWriteMask == BB.ColorWriteMask;

		if(!bSame)
			break;
	}

	return bSame;
}

bool operator==(const FRHIVertexElement& A, const FRHIVertexElement& B)
{
	bool bSame = A.Type == B.Type &&
		A.StreamIndex == B.StreamIndex &&
		A.AttributeIndex == B.AttributeIndex &&
		A.Offset == B.Offset &&
		A.Stride == B.Stride;
	return bSame;
}

bool operator==(const FRHIVertexInputStateDesc& A, const FRHIVertexInputStateDesc& B)
{
	bool bSame = A.NumVertexElements == B.NumVertexElements;
	if(!bSame)
		return false;
	for (uint8 i = 0; i < A.NumVertexElements; i++)
	{
		bSame &= A.VertexElements[i] == B.VertexElements[i];
		if (!bSame)
			break;
	}
	return bSame;
}



uint8 FRHIVertexElement::GetComponentSize()
{
	switch (Type)
	{
	case EVertexElmentType::Float1:   return 4;
	case EVertexElmentType::Float2:   return 4 * 2;
	case EVertexElmentType::Float3:   return 4 * 3;
	case EVertexElmentType::Float4:   return 4 * 4;
	case EVertexElmentType::UByte4:   return 1 * 4;
	case EVertexElmentType::Color:	  return 1 * 4;
	case EVertexElmentType::Matrix3:  return 4 * 3 * 3;
	case EVertexElmentType::Matrix4:  return 4 * 4 * 4;
	case EVertexElmentType::UInt:     return 4;
	case EVertexElmentType::Bool:     return 1;
	}

	REV_CORE_ASSERT(false, "Unknown VertexElementType!");
	return 0;
}

uint8 FRHIVertexElement::GetComponentCount()
{
	switch (Type)
	{
	case EVertexElmentType::Float1:  return 1;
	case EVertexElmentType::Float2:  return 2;
	case EVertexElmentType::Float3:  return 3;
	case EVertexElmentType::Float4:  return 4;
	case EVertexElmentType::UByte4:  return 4;
	case EVertexElmentType::Color:	 return 4;
	case EVertexElmentType::Matrix3: return 3; // 3* float3
	case EVertexElmentType::Matrix4: return 4; // 4* float4
	case EVertexElmentType::UInt:    return 1;
	case EVertexElmentType::Bool:    return 1;
	}

	REV_CORE_ASSERT(false, "Unknown VertexElementType!");
	return 0;
}

uint32 FRHIVertexElement::GetElementSize()
{
	return uint32(GetComponentSize()) * uint32(GetComponentCount());
}


}