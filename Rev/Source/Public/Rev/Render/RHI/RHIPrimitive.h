#pragma once
#include "Rev/Core/Base.h"

namespace Rev
{

enum EPrimitiveTopology : uint8
{
	PT_Unknown = 0,
	PT_Points,
	PT_Lines,
	PT_LineLoop,
	PT_LineStrip,
	PT_Triangles,
	PT_TriangleStrip,
	PT_TriangleFan
};

}