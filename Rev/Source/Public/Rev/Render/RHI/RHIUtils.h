#pragma once
#include "Rev/Render/RHI/RHIDefinitions.h"

namespace Rev
{

inline uint32 ComputeVertexCount(uint32 NumPrimitives, uint32 PrimitiveType)
{
	uint32 Stride = (PrimitiveType == PT_Triangles) ? 3 : (PrimitiveType == PT_Lines) ? 2 : 1;
	uint32 Offset = (PrimitiveType == PT_TriangleStrip) ? 2 : 0;

	return NumPrimitives * Stride + Offset;
}

}