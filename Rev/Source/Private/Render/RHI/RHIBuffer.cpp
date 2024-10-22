#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

uint8 FRHIVertexElement::ElementSize(EVertexType InType)
{
	switch (InType)
	{
	case EVertexType::Float:    return 4;
	case EVertexType::Float2:   return 4 * 2;
	case EVertexType::Float3:   return 4 * 3;
	case EVertexType::Float4:   return 4 * 4;
	case EVertexType::Matrix3:  return 4 * 3 * 3;
	case EVertexType::Matrix4:  return 4 * 4 * 4;
	case EVertexType::Int:      return 4;
	case EVertexType::Int2:     return 4 * 2;
	case EVertexType::Int3:     return 4 * 3;
	case EVertexType::Int4:     return 4 * 4;
	case EVertexType::Bool:     return 1;
	}

	REV_CORE_ASSERT(false, "Unknown VertexElementType!");
	return 0;
}

uint8 FRHIVertexElement::ComponentCount(EVertexType InType)
{
	switch (InType)
	{
	case EVertexType::Float:   return 1;
	case EVertexType::Float2:  return 2;
	case EVertexType::Float3:  return 3;
	case EVertexType::Float4:  return 4;
	case EVertexType::Matrix3: return 3; // 3* float3
	case EVertexType::Matrix4: return 4; // 4* float4
	case EVertexType::Int:     return 1;
	case EVertexType::Int2:    return 2;
	case EVertexType::Int3:    return 3;
	case EVertexType::Int4:    return 4;
	case EVertexType::Bool:    return 1;
	}

	REV_CORE_ASSERT(false, "Unknown VertexElementType!");
	return 0;
}

}