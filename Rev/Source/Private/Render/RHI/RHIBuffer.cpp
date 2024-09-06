#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

uint32 VertexElementSize(EVertexElementType type)
{
	switch (type)
	{
	case EVertexElementType::Float:    return 4;
	case EVertexElementType::Float2:   return 4 * 2;
	case EVertexElementType::Float3:   return 4 * 3;
	case EVertexElementType::Float4:   return 4 * 4;
	case EVertexElementType::Mat3:     return 4 * 3 * 3;
	case EVertexElementType::Mat4:     return 4 * 4 * 4;
	case EVertexElementType::Int:      return 4;
	case EVertexElementType::Int2:     return 4 * 2;
	case EVertexElementType::Int3:     return 4 * 3;
	case EVertexElementType::Int4:     return 4 * 4;
	case EVertexElementType::Bool:     return 1;
	}

	REV_CORE_ASSERT(false, "Unknown VertexElementType!");
	return 0;
}

uint32 VertexComponentCount(EVertexElementType type)
{
	switch (type)
	{
	case EVertexElementType::Float:   return 1;
	case EVertexElementType::Float2:  return 2;
	case EVertexElementType::Float3:  return 3;
	case EVertexElementType::Float4:  return 4;
	case EVertexElementType::Mat3:    return 3; // 3* float3
	case EVertexElementType::Mat4:    return 4; // 4* float4
	case EVertexElementType::Int:     return 1;
	case EVertexElementType::Int2:    return 2;
	case EVertexElementType::Int3:    return 3;
	case EVertexElementType::Int4:    return 4;
	case EVertexElementType::Bool:    return 1;
	}

	REV_CORE_ASSERT(false, "Unknown VertexElementType!");
	return 0;
}

uint32 FRHIVertexElement::GetElementSize() const
{
	return VertexElementSize(Type);
}

uint32 FRHIVertexElement::GetComponentCount() const
{
	return VertexComponentCount(Type);
}

}