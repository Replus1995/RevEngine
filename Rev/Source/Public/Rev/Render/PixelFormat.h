#pragma once
#include "Rev/Core/Base.h"

namespace Rev
{

enum EPixelFormat : uint8
{
	PF_Unknown = 0,
	PF_R32G32B32A32F = 1,
	PF_R8G8B8A8 = 2,
	PF_R8 = 3,
	PF_R16 = 4,
	PF_RGB8 = 5,
	PF_DepthStencil = 6,
	PF_ShadowDepth = 7
};


}