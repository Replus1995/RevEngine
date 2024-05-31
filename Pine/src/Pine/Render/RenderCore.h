#pragma once
#include <memory>

namespace Pine
{

enum class ERenderAPI : uint8_t
{
    None = 0,
    OpenGL = 1,
    Vulkan = 2
};

}