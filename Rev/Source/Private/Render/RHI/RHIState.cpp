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