#include "Rev/Render/RHI/RHIState.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

bool FRHISamplerStateDesc::operator==(const FRHISamplerStateDesc& B) const
{
	bool bSame = Filter == B.Filter &&
		WarpU == B.WarpU &&
		WarpV == B.WarpV &&
		WarpW == B.WarpW &&
		MipBias == B.MipBias &&
		MinMipLevel == B.MinMipLevel &&
		MaxMipLevel == B.MaxMipLevel &&
		MaxAnisotropy == B.MaxAnisotropy &&
		BorderColor == B.BorderColor &&
		CompareFunc == B.CompareFunc;
	return bSame;
}

bool FRHIRasterizerStateDesc::operator==(const FRHIRasterizerStateDesc& B) const
{
	bool bSame = FillMode == B.FillMode &&
		CullMode == B.CullMode &&
		DepthBias == B.DepthBias &&
		DepthBiasSlopeFactor == B.DepthBiasSlopeFactor &&
		DepthClipMode == B.DepthClipMode &&
		bAllowMSAA == B.bAllowMSAA;
	return bSame;
}

bool FRHIDepthStencilStateDesc::operator==(const FRHIDepthStencilStateDesc& B) const
{
	bool bSame = bEnableDepthWrite == B.bEnableDepthWrite &&
		DepthTestFunc == B.DepthTestFunc &&
		bEnableFrontFaceStencil == B.bEnableFrontFaceStencil &&
		bEnableBackFaceStencil == B.bEnableBackFaceStencil &&
		FrontFaceStencilFailOp == B.FrontFaceStencilFailOp &&
		FrontFaceStencilPassOp == B.FrontFaceStencilPassOp &&
		FrontFaceStencilDepthFailOp == B.FrontFaceStencilDepthFailOp &&
		FrontFaceStencilFunc == B.FrontFaceStencilFunc &&
		BackFaceStencilFailOp == B.BackFaceStencilFailOp &&
		BackFaceStencilPassOp == B.BackFaceStencilPassOp &&
		BackFaceStencilDepthFailOp == B.BackFaceStencilDepthFailOp &&
		BackFaceStencilFunc == B.BackFaceStencilFunc &&
		StencilReadMask == B.StencilReadMask &&
		StencilWriteMask == B.StencilWriteMask;
	return bSame;
}

bool FRHIColorBlendStateDesc::operator==(const FRHIColorBlendStateDesc& B) const
{
	bool bSame = bUseAlphaToCoverage == B.bUseAlphaToCoverage;
	for (uint8 i = 0; i < RTA_MaxColorAttachments; i++)
	{
		const FRHIColorBlendStateDesc::FAttachment& AA = Attachments[i];
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

bool FRHIVertexElement::operator==(const FRHIVertexElement& B) const
{
	bool bSame = Type == B.Type &&
		StreamIndex == B.StreamIndex &&
		AttributeIndex == B.AttributeIndex &&
		Offset == B.Offset &&
		Stride == B.Stride;
	return bSame;
}

bool FRHIVertexInputStateDesc::operator==(const FRHIVertexInputStateDesc& B) const
{
	bool bSame = NumVertexElements == B.NumVertexElements;
	if(!bSame)
		return false;
	for (uint8 i = 0; i < NumVertexElements; i++)
	{
		bSame &= VertexElements[i] == B.VertexElements[i];
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