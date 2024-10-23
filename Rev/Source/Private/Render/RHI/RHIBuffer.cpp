#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

uint8 FRHIVertexElement::ElementSize(EVertexElmentType InType)
{
	switch (InType)
	{
	case EVertexElmentType::Float:    return 4;
	case EVertexElmentType::Float2:   return 4 * 2;
	case EVertexElmentType::Float3:   return 4 * 3;
	case EVertexElmentType::Float4:   return 4 * 4;
	case EVertexElmentType::Matrix3:  return 4 * 3 * 3;
	case EVertexElmentType::Matrix4:  return 4 * 4 * 4;
	case EVertexElmentType::Int:      return 4;
	case EVertexElmentType::Int2:     return 4 * 2;
	case EVertexElmentType::Int3:     return 4 * 3;
	case EVertexElmentType::Int4:     return 4 * 4;
	case EVertexElmentType::Bool:     return 1;
	}

	REV_CORE_ASSERT(false, "Unknown VertexElementType!");
	return 0;
}

uint8 FRHIVertexElement::ComponentCount(EVertexElmentType InType)
{
	switch (InType)
	{
	case EVertexElmentType::Float:   return 1;
	case EVertexElmentType::Float2:  return 2;
	case EVertexElmentType::Float3:  return 3;
	case EVertexElmentType::Float4:  return 4;
	case EVertexElmentType::Matrix3: return 3; // 3* float3
	case EVertexElmentType::Matrix4: return 4; // 4* float4
	case EVertexElmentType::Int:     return 1;
	case EVertexElmentType::Int2:    return 2;
	case EVertexElmentType::Int3:    return 3;
	case EVertexElmentType::Int4:    return 4;
	case EVertexElmentType::Bool:    return 1;
	}

	REV_CORE_ASSERT(false, "Unknown VertexElementType!");
	return 0;
}

}