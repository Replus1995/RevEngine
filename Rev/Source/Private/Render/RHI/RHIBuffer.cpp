#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Core/Assert.h"

namespace Rev
{

uint8 FRHIVertexElement::ElementSize(EVertexType InType)
{
	switch (InType)
	{
	case VT_Float:    return 4;
	case VT_Float2:   return 4 * 2;
	case VT_Float3:   return 4 * 3;
	case VT_Float4:   return 4 * 4;
	case VT_Mat3:     return 4 * 3 * 3;
	case VT_Mat4:     return 4 * 4 * 4;
	case VT_Int:      return 4;
	case VT_Int2:     return 4 * 2;
	case VT_Int3:     return 4 * 3;
	case VT_Int4:     return 4 * 4;
	case VT_Bool:     return 1;
	}

	REV_CORE_ASSERT(false, "Unknown VertexElementType!");
	return 0;
}

uint8 FRHIVertexElement::ComponentCount(EVertexType InType)
{
	switch (InType)
	{
	case VT_Float:   return 1;
	case VT_Float2:  return 2;
	case VT_Float3:  return 3;
	case VT_Float4:  return 4;
	case VT_Mat3:    return 3; // 3* float3
	case VT_Mat4:    return 4; // 4* float4
	case VT_Int:     return 1;
	case VT_Int2:    return 2;
	case VT_Int3:    return 3;
	case VT_Int4:    return 4;
	case VT_Bool:    return 1;
	}

	REV_CORE_ASSERT(false, "Unknown VertexElementType!");
	return 0;
}

}