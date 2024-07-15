#pragma once
#include "Rev/Core/Base.h"
#include <memory>

namespace Rev
{

enum class ERenderAPI : uint8_t
{
    None = 0,
    OpenGL = 1,
    Vulkan = 2
};
ERenderAPI GetRenderAPI();

enum EBlendMode : uint8
{
    BM_Opaque = 0,
    BM_Transparent,
    BM_Masked
};

enum EShadingModel : uint8
{
    SM_Default,
    SM_Unlit
};

}