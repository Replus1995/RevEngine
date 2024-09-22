#pragma once
#include "Rev/Core/Base.h"
#include <memory>

namespace Rev
{

enum class ERenderAPI : uint8
{
    None = 0,
    OpenGL = 1,
    Vulkan = 2
};
REV_API ERenderAPI GetRenderAPI();

enum EDrawMode : uint8
{
	DM_Unknown = 0,
	DM_Points,
	DM_Line,
	DM_LineLoop,
	DM_LineStrip,
	DM_Triangles,
	DM_TriangleStrip,
	DM_TriangleFan
};

enum EBlendMode : uint8
{
	BM_Diabled = 0,
    BM_Opaque = 1,
    BM_Transparent,
    BM_Masked
};

enum EShadingModel : uint8
{
    SM_Default = 0,
    SM_Unlit
};

enum ECullFaceMode : uint8
{
	CFM_Disabled = 0,
	CFM_Back,
	CFM_Front,
	CFM_BackAndFront
};

enum EDepthTestMode : uint8
{
	DTM_Never = 0,
	DTM_Less,
	DTM_Equal,
	DTM_Greater,
	DTM_NotEqual,
	DTM_LessEqual,
	DTM_GreaterEqual,
	DTM_Always
};

}

#define REV_MAX_DIRECTIONAL_LIGHTS 4
#define REV_MAX_POINT_LIGHTS 16
#define REV_MAX_SPOT_LIGHTS 16
