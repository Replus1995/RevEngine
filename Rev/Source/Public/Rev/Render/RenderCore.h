#pragma once
#include "Rev/Core/Base.h"
#include <memory>

#define REV_MAX_DIRECTIONAL_LIGHTS 4
#define REV_MAX_POINT_LIGHTS 16
#define REV_MAX_SPOT_LIGHTS 16

namespace Rev
{

enum class ERenderAPI : uint8
{
    None = 0,
    Vulkan = 1
};
REV_API ERenderAPI GetRenderAPI();

}
