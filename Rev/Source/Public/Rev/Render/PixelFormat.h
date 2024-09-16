#pragma once
#include "Rev/Core/Base.h"

namespace Rev
{

enum EPixelFormat : uint32
{
	PF_Unknown = 0,
	PF_R32G32B32A32F,
	PF_R8G8B8A8,
	PF_R8,
	PF_R16,
	PF_RGB8,
	PF_DepthStencil,
	PF_ShadowDepth
};

struct FPixelFormatInfo
{
	EPixelFormat Format;
	uint16 Channels;
	uint16 PixelDepth;
};

class FPixelFormatUtils
{
public:
	static FPixelFormatInfo GetFormatInfo(EPixelFormat InFormat);
	static bool HasDepth(EPixelFormat InFormat);
	static bool HasStencil(EPixelFormat InFormat);
};


}