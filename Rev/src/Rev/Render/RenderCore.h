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

}